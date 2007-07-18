#include <boost/thread/thread.hpp>
#include <errno.h>
#include "network.h"
#include "datareceiver.h"

Network::Network(std::string somaIP) :
  running_ (false), 
  pthrd_(NULL), 
  pDispatch_(new EventDispatcher()), 
  eventReceiver_(pDispatch_, 
		 boost::bind(&Network::appendEventOut, this, _1) ),
  eventSender_(pDispatch_, somaIP)
{


}

void Network::run()
{
  running_ = true; 
  pthrd_ = new boost::thread(boost::bind(&Network::workthread,
					 this));
  
}
void Network::workthread()
{
  pDispatch_->run(); 
  
}

void Network::shutdown()
{
  running_ = false; 
  pDispatch_->halt(); 

}

Network::~Network()
{
  shutdown(); 
  pthrd_->join(); 

  // delete the data receivers that remain
  std::map<const datagen_t, DataReceiver*>::iterator i;
  for (i = dataReceivers_.begin(); i != dataReceivers_.end(); 
       i++) 
    {
      
      delete (*i).second; 
    }
  
  
  // 

}


void Network::appendDataOut(DataPacket_t* out) {
  
  outputDataFifo_.append(out); 
  
}

void Network::appendEventOut(EventList_t* out) {

  outputEventFifo_.append(out); 
  
}

DataPacket_t* Network::getNewData(void)
{
  return outputDataFifo_.pop(); 
}

EventList_t* Network::getNewEvents(void)
{
  return outputEventFifo_.pop(); 
}

int Network::getDataFifoPipe()
{
  return outputDataFifo_.readingPipe; 
}

int Network::getEventFifoPipe()
{
  return outputEventFifo_.readingPipe; 
}

void Network::enableDataRX(datasource_t src, datatype_t typ)
{

  datagen_t dg(src, typ); 

  dataReceivers_[dg] = new DataReceiver(pDispatch_, src, typ, 
  					boost::bind(&Network::appendDataOut, 
  						    this, _1) ); 
  
  
}

void Network::disableDataRX(datasource_t src, datatype_t typ)
{
  // we really need a custom try/catch here 
  datagen_t dg(src, typ); 
  DataReceiver* dr = dataReceivers_[dg]; 
  std::map<const datagen_t, DataReceiver*>::iterator i = dataReceivers_.find(dg); 
  // UM THIS IS BROKEN WHAT ARE WE SUPPOSED TO DO WITH i? 

  delete dr;   
}

std::vector<DataReceiverStats>  Network::getDataStats()
{

  std::map<const datagen_t, DataReceiver*>::iterator i;
  std::vector<DataReceiverStats> drs; 
  for (i = dataReceivers_.begin(); i != dataReceivers_.end(); i++)
    {
      drs.push_back(((i->second))->getStats()); 
    }
  return std::vector<DataReceiverStats>(drs); 
  
}

eventtxnonce_t Network::sendEvents(const EventTXList_t & el)
{
  eventSender_.sendEvents(el);  // the eventSender is thread-safe, so 
  // this can be called from external functions

}

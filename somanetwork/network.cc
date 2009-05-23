#include <boost/thread/thread.hpp>
#include <errno.h>
#include "network.h"
#include "datareceiver.h"
#include "netsockproxy.h"
#include "sockproxy.h"

namespace somanetwork { 
  
  pNetworkInterface_t Network::createINet(std::string somaip) 
  {
    
    pISocketProxy_t sp(new NetSocketProxy(somaip)); 
    return pNetworkInterface_t(new Network(sp)); 
  }
  
  pNetworkInterface_t Network::createDomain(std::string rootdir) {

    throw std::runtime_error("Not implemented"); 
    
  }

Network::Network(pISocketProxy_t sockprox) :
  pSockProxy_(sockprox), 
  running_ (false), 
  pthrd_(NULL), 
  pDispatch_(new EventDispatcher()), 
  eventReceiver_(pDispatch_, sockprox, 
		 boost::bind(&Network::appendEventOut, this, _1) ),
  eventSender_(pDispatch_, sockprox)
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
  //std::cout << "NETWORK SHUTDOWN" << std::endl; 
  running_ = false; 
  pDispatch_->halt(); 

}

Network::~Network()
{
  shutdown(); 
  if (running_) {
    pthrd_->join(); 
  }
  // delete the data receivers that remain
  std::map<const datagen_t, DataReceiver*>::iterator i;
  for (i = dataReceivers_.begin(); i != dataReceivers_.end(); 
       i++) 
    {
      
      delete (*i).second; 
    }

  
}

void Network::disableAllDataRX()
{
  std::map<const datagen_t, DataReceiver*>::iterator i;
  for (i = dataReceivers_.begin(); i != dataReceivers_.end(); 
       i++) 
    {
      
      delete (*i).second; 
    }
  dataReceivers_.clear(); 

}

void Network::appendDataOut(pDataPacket_t out) {
  
  outputDataFifo_.append(out); 
  
}

void Network::appendEventOut(pEventPacket_t out) {

  outputEventFifo_.append(out); 
  
}

pDataPacket_t Network::getNewData(void)
{
  return outputDataFifo_.pop(); 
}

pEventPacket_t Network::getNewEvents(void)
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

  dataReceivers_[dg] = new DataReceiver(pDispatch_, 
					pSockProxy_,src, typ, 
  					boost::bind(&Network::appendDataOut, 
  						    this, _1) ); 
  
  
}

void Network::disableDataRX(datasource_t src, datatype_t typ)
{
  // we really need a custom try/catch here 
  datagen_t dg(src, typ); 
  DataReceiver* dr = dataReceivers_[dg]; 
  std::map<const datagen_t, DataReceiver*>::iterator i = dataReceivers_.find(dg); 
  dataReceivers_.erase(i); 
  
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
  return drs; 
  
}

SeqPacketProtoStats Network::getEventStats()
{

  return eventReceiver_.getStats(); 

}

eventtxnonce_t Network::sendEvents(const EventTXList_t & el)
{
  eventSender_.sendEvents(el);  // the eventSender is thread-safe, so 
  // this can be called from external functions

}

void Network::resetDataStats() 
{
  // FIXME

}


void Network::resetEventStats()
{
  // FIXME

}


}

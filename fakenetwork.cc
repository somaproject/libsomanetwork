#include <boost/thread/thread.hpp>
#include <errno.h>
#include <unistd.h>
#include "network.h"
#include "datareceiver.h"


const int EPOLLMAXCNT = 256; 

FakeNetwork::FakeNetwork() :
  running_ (false), 
  pthrd_(NULL)
{


}

void FakeNetwork::run()
{
  running_ = true; 
  pthrd_ = new boost::thread(boost::bind(&FakeNetwork::workthread,
					 this));
  
}
void FakeNetwork::workthread()
{
  while(running_) {
    // we just spinlock
    usleep(100000); 

  }
  
}

void FakeNetwork::shutdown()
{
  running_ = false; 

}

FakeNetwork::~FakeNetwork()
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


void FakeNetwork::appendDataOut(DataPacket_t* out) {

  outputDataFifo_.append(out); 
  
}

void FakeNetwork::appendEventOut(EventList_t* out) {

  outputEventFifo_.append(out); 
  
}

DataPacket_t* FakeNetwork::getNewData(void)
{
  return outputDataFifo_.pop(); 
}

EventList_t* FakeNetwork::getNewEvents(void)
{
  return outputEventFifo_.pop(); 
}

int FakeNetwork::getDataFifoPipe()
{
  return outputDataFifo_.readingPipe; 
}

int FakeNetwork::getEventFifoPipe()
{
  return outputEventFifo_.readingPipe; 
}

void FakeNetwork::enableDataRX(datasource_t src, datatype_t typ)
{

}

void FakeNetwork::disableDataRX(datasource_t src, datatype_t typ)
{

}

std::vector<DataReceiverStats>  FakeNetwork::getDataStats()
{

  std::map<const datagen_t, DataReceiver*>::iterator i;
  std::vector<DataReceiverStats> drs; 
  return std::vector<DataReceiverStats>(drs); 
  
}

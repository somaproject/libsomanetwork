#include <boost/thread/thread.hpp>
#include <errno.h>
#include <unistd.h>
#include "fakenetwork.h"


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


}


void FakeNetwork::appendDataOut(pDataPacket_t out) {
  boost::mutex::scoped_lock lock(appendMutex_);
  
  outputDataFifo_.append(out); 
  
}

void FakeNetwork::appendEventOut(EventList_t* out) {
  boost::mutex::scoped_lock lock(appendMutex_);

  outputEventFifo_.append(out); 
  
}

pDataPacket_t FakeNetwork::getNewData(void)
{
  boost::mutex::scoped_lock lock(appendMutex_);
  
  pDataPacket_t  dp =  outputDataFifo_.pop(); 

  return dp; 

}

EventList_t* FakeNetwork::getNewEvents(void)
{
  boost::mutex::scoped_lock lock(appendMutex_);

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

eventtxnonce_t FakeNetwork::sendEvents(const EventTXList_t & el)
{
  // here's where we need to do actual work to place these on the relevant 
  // bus

  signalEventTX_.emit(el); 


}

signalEventTX_t & FakeNetwork::signalEventTX()
{

  return signalEventTX_; 

}

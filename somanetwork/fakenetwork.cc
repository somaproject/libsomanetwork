#include <boost/thread/thread.hpp>
#include <errno.h>
#include <unistd.h>
#include "fakenetwork.h"

namespace somanetwork {

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
  if (pthrd_) {
    pthrd_->join(); 
  }

}


void FakeNetwork::appendDataOut(pDataPacket_t out) {
  boost::mutex::scoped_lock lock(appendMutex_);
  bool isvalid = false; 
  for (std::set<datagen_t>::iterator mp = dataReceivers_.begin(); 
       mp != dataReceivers_.end(); mp++) {
    if (mp->first == out->src and mp->second == out->typ) {
      
      outputDataFifo_.append(out); 
      break; 
    }
  }

}

void FakeNetwork::appendEventOut(pEventPacket_t out) {
  boost::mutex::scoped_lock lock(appendMutex_);

  outputEventFifo_.append(out); 
  
}

pDataPacket_t FakeNetwork::getNewData(void)
{
  boost::mutex::scoped_lock lock(appendMutex_);
  
  pDataPacket_t  dp =  outputDataFifo_.pop(); 

  return dp; 

}

pEventPacket_t FakeNetwork::getNewEvents(void)
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
  dataReceivers_.insert(std::make_pair(src, typ)); 
}

void FakeNetwork::disableDataRX(datasource_t src, datatype_t typ)
{
  dataReceivers_.erase(std::make_pair(src, typ)); 

}

std::vector<DataReceiverStats>  FakeNetwork::getDataStats()
{

  return currentStats_; 
  
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


void FakeNetwork::disableAllDataRX()
{
  dataReceivers_.clear(); 
}


void FakeNetwork::setDataStats(std::vector<DataReceiverStats> ds)
{

  currentStats_ = ds; 
}


  
void FakeNetwork::resetDataStats() 
{


}


void FakeNetwork::resetEventStats()
{


}


EventReceiverStats FakeNetwork::getEventStats() 
{


}

}

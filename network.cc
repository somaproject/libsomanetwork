#include <boost/thread/thread.hpp>
#include <errno.h>
#include "network.h"
#include "datareceiver.h"


const int EPOLLMAXCNT = 256; 



Network::Network() :
  running_ (false), 
  pthrd_(NULL), 
  epollfd_(epoll_create(EPOLLMAXCNT)),
  eventReceiver_(epollfd_, 
		 boost::bind(&Network::appendEventOut, this, _1) )
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
  while(running_) {

    epoll_event events[EPOLLMAXCNT]; 
    const int epMaxWaitMS = 1; 
    int nfds = epoll_wait(epollfd_, events, EPOLLMAXCNT, 
			  epMaxWaitMS); 


    if (nfds > 0 ) {
      
      for(int evtnum = 0; evtnum < nfds; evtnum++) {
	// we treat each callback as ... ummm... 
	boost::function<void> * func =  (boost::function<void> *)events[evtnum].data.ptr; 
	(*func)(); 
	
      }

    } else if (nfds < 0 ) {
      if (errno == EINTR) {
	std::cerr << "EINTR: The call was interrupted by a " 
		  << "singal handler before any of the requested events "
		  << "occured or THE TIMEOUT EXPIRED" << std::endl; 

      } else {
	throw std::runtime_error("epoll_wait returned an unexpected error condition"); 
      }
    } else {
      // otherwise, just a timeout
    }

  }
  
}

void Network::shutdown()
{
  running_ = false; 

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

  if (running_) {
    std::runtime_error("cannot change DataRx state while running"); 
  }


   dataReceivers_[dg] = new DataReceiver(epollfd_, src, typ, 
  					boost::bind(&Network::appendDataOut, 
  						    this, _1) ); 
  
  
}

void Network::disableDataRX(datasource_t src, datatype_t typ)
{
  if (running_) {
    throw std::runtime_error("cannot change DataRx state while running"); 
  }

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

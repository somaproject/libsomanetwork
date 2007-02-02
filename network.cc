#include <boost/thread/thread.hpp>
#include <errno.h>
#include "network.h"
#include "datareceiver.h"


const int EPOLLMAXCNT = 64; 

Network::Network() :
  shuttingDown_ (false)
{
  epollfd_ = epoll_create(EPOLLMAXCNT); 

}

void Network::run()
{
  pthrd_ = new boost::thread(boost::bind(&Network::workthread,
					 this));
  
}
void Network::workthread()
{
  while(not shuttingDown_) {


    epoll_event events[EPOLLMAXCNT]; 
    const int epMaxWaitMS = 1000; 
    int nfds = epoll_wait(epollfd_, events, EPOLLMAXCNT, 
			  epMaxWaitMS); 


    if (nfds > 0 ) {
      
      for(int evtnum = 0; evtnum < nfds; evtnum++) {
	DataReceiver * drp  = (DataReceiver*)events[evtnum].data.ptr; 
	//std::cout << (long int)events[evtnum].data.ptr << std::endl; 
	drp->handleReceive(); 
      }


    } else if (nfds < 0 ) {
      if (errno == EINTR) {
	std::cerr << "EINTR: The call was interrupted by a singal handler before any of the requested events occured or the timeout expired" << std::endl; 

      }
    } else {
      std::cout << "Timeout occureD" << std::endl;
      // otherwise, just a timeout
    }

  }
  
}

void Network::shutdown()
{
  shuttingDown_ = true; 

}

Network::~Network()
{
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


void Network::appendDataOut(RawData* out) {
  //std::cout << "appending" << std::endl; 
  outputFifo_.append(out); 
  
}

RawData* Network::getNewData(void)
{
  return outputFifo_.pop(); 
}

int Network::getTSPipeFifoPipe()
{
  return outputFifo_.readingPipe; 
}

void Network::enableDataRx(datasource_t src, datatype_t typ)
{
  datagen_t dg(src, typ); 

  dataReceivers_[dg] = new DataReceiver(epollfd_, src, typ, 
					boost::bind(&Network::appendDataOut, 
						    this, _1) ); 
  
  
}

void Network::disableDataRx(datasource_t src, datatype_t typ)
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

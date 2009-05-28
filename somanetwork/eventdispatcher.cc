#include "eventdispatcher.h"
#include <boost/format.hpp>

namespace somanetwork {

EventDispatcher::EventDispatcher() :
  epollFD_(epoll_create(EPOLLMAXCNT))
{
  
  // setup control endpoint
  int pipes[2]; 
  int result = pipe(pipes); 
  if (result < 0) {
    throw std::runtime_error("Error opening pipe for event dispatch"); 
  }
  controlFDw_ = pipes[1]; 
  controlFDr_ = pipes[0]; 
  addEvent(controlFDr_,
	   boost::bind(std::mem_fun(&EventDispatcher::controlEvent), this, _1)); 
  
}

EventDispatcher::~EventDispatcher()
{
  


}

void EventDispatcher::addEvent(int fd, eventCallback_t cb)
{
  
  {
    boost::mutex::scoped_lock lock( cbTableMutex_ );
    
    callbackTable_[fd] = cb; 
  } 
  struct epoll_event  ev; 
  bzero(&ev, sizeof(ev)); 
  // try adding to epoll
  ev.events = EPOLLIN; 
  ev.data.fd = fd;
  int errorret = epoll_ctl(epollFD_, EPOLL_CTL_ADD, fd, &ev); 
  if (errorret == -1) {
    int errsv = errno; 
    boost::format errorstr("could not add FD to epoll event set, err = '%s'"); 
    
    throw std::runtime_error(boost::str(errorstr % strerror(errsv))); 
    
  }
  
}

void EventDispatcher::delEvent(int fd)
{

  struct epoll_event  ev; 
  ev.events = EPOLLIN; 
  ev.data.fd = fd;

  int errorret = epoll_ctl(epollFD_, EPOLL_CTL_DEL, fd, &ev); 

  if (errorret != 0 ) {
    int errsv = errno; 
    boost::format errorstr("could not delete FD from epoll event set, err = '%s'"); 
    throw std::runtime_error(boost::str(errorstr % strerror(errsv)));
  }
  
  boost::mutex::scoped_lock lock( cbTableMutex_ );
  
  callbackTable_.erase(fd);
  
}

// now the part we actually care about:

void EventDispatcher::run(void)
{
  running_ = true; 
  while(running_)
    {

      runonce(1); 
    }
}


  
void EventDispatcher::controlEvent(int fd)
{
  running_ = false; 

}

void EventDispatcher::halt()
{
  char x(0); 
  int result = write(controlFDw_, &x, 1); 

}

  void EventDispatcher::runonce(int epMaxWaitMS)
  {
    
    
    epoll_event events[EPOLLMAXCNT]; 

    int nfds = epoll_wait(epollFD_, events, EPOLLMAXCNT, 
			  epMaxWaitMS); 
    
    
      if (nfds > 0 ) {
	boost::mutex::scoped_lock lock( cbTableMutex_ );
	
	for(int evtnum = 0; evtnum < nfds; evtnum++) {
	  int fd = events[evtnum].data.fd; 
	  
	  callbackTable_[fd](fd); 
	}
	
      } else if (nfds < 0 ) {
	if (errno == EINTR) {
	  std::cerr << "EINTR: The call was interrupted by a " 
		    << "singal handler before any of the requested events "
		    << "occured or THE TIMEOUT EXPIRED" << std::endl; 
	  
	} else {
	  throw std::runtime_error("epoll_wait returned an unexpected error condition"); 
	}
      }
      
      boost::mutex::scoped_lock lock( cbTimeoutsMutex_ );
      for(callbackList_t::iterator i = timeouts_.begin(); i != timeouts_.end(); i++)
	{
	  (*i)(0); 
	}
      
  }
  
void EventDispatcher::addTimeout(eventCallback_t cb)
{

  boost::mutex::scoped_lock lock( cbTimeoutsMutex_ );
  timeouts_.push_back(cb); 

}

void EventDispatcher::delTimeout(eventCallback_t cb)
{
  boost::mutex::scoped_lock lock( cbTimeoutsMutex_ );
  // now find and delete the callback; is O(n); 
//   callbackList_t::iterator i = find(timeouts_.begin(), timeouts_.end(), 
// 				    cb); 
//   if (i == timeouts_.end() )
//     {
//       throw std::runtime_error("requested callback was not present in timeout list"); 
//     }
//   timeouts_.erase(i); 
	
}

}

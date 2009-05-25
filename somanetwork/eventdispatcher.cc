#include "eventdispatcher.h"

#include <event.h>
typedef struct event libevent_event_t;


namespace somanetwork {

EventDispatcher::EventDispatcher() {
  
    event_init();
    
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
  

static void generic_event_callback(int fd, short evt, void *arg){

    EventDispatcher *ed = (EventDispatcher *)arg;
    ed->dispatchEvent(fd);
}

void EventDispatcher::addEvent(int fd, eventCallback_t cb)
{
  
  {
    boost::mutex::scoped_lock lock( cbTableMutex_ );
    
    callbackTable_[fd] = cb; 
  } 
  
    struct event *ev = new struct event();
    
    event_set(ev, fd, EV_READ | EV_PERSIST, generic_event_callback, this);
    event_add(ev, NULL);
    
    {
        boost::mutex::scoped_lock lock( eventTableMutex_ );
        eventTable_[fd] = ev;
    }
    
}

void EventDispatcher::delEvent(int fd)
{

    struct event *ev;
    {
        boost::mutex::scoped_lock lock( eventTableMutex_ );
        ev = eventTable_[fd];
    }
    
    event_del(ev);
    
  
  boost::mutex::scoped_lock lock( cbTableMutex_ );
  
  callbackTable_.erase(fd);
    eventTable_.erase(fd);
  
}

// now the part we actually care about:

void EventDispatcher::run(void)
{

  running_ = true; 
    
    
  while(running_){
      runonce();
  }
  event_loopexit(NULL);
    
}

void EventDispatcher::runonce(void)
{
    event_loop(EVLOOP_NONBLOCK | EVLOOP_ONCE);
}
  
void EventDispatcher::controlEvent(int fd)
{
  running_ = false; 

}

void EventDispatcher::halt()
{
  char x; 
  int result = write(controlFDw_, &x, 1); 

}
    

void EventDispatcher::dispatchEvent(int fd){
 //   int fd = event_to_dispatch->ev_fd; 
//    
    callbackTable_[fd](fd);
}

/*void EventDispatcher::runonce()
{


      epoll_event events[EPOLLMAXCNT]; 
      const int epMaxWaitMS = 1; 
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
      
}*/

void EventDispatcher::addTimeout(eventCallback_t cb)
{

  boost::mutex::scoped_lock lock( cbTimeoutsMutex_ );
  timeouts_.push_back(cb); 

}

void EventDispatcher::delTimeout(eventCallback_t cb)
{
//  boost::mutex::scoped_lock lock( cbTimeoutsMutex_ );
//  // now find and delete the callback; is O(n); 
//   callbackList_t::iterator i = find(timeouts_.begin(), timeouts_.end(), cb); 
//   if (i == timeouts_.end() )
//     {
//       throw std::runtime_error("requested callback was not present in timeout list"); 
//     }
//   timeouts_.erase(i); 
	
}
}


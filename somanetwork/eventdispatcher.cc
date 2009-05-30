#include "eventdispatcher.h"
#include <boost/format.hpp>

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
		// DDC: Note, I'm not running "runonce()" here so that I can let libevent
		// do it's thing without artificial timeout periods.  The runonce() remains
		// however, because the tests (and possibly other code) need it in order to 
		// work correctly
		event_loop(EVLOOP_NONBLOCK | EVLOOP_ONCE);
  }
    
}

void EventDispatcher::runonce(int timeout_ms)
{
    // todo -- use that new timeout
  struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = timeout_ms * 1000;
	event_loopexit(&timeout);
	event_loop(0);
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
    

void EventDispatcher::dispatchEvent(int fd){
    callbackTable_[fd](fd);
}


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


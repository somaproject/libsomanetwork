#ifndef SOMANETWORK_EVENTDISPATCHER_H
#define SOMANETWORK_EVENTDISPATCHER_H

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <utility>
//#include <sys/epoll.h>
#include <errno.h>
#include <iostream>
#include <list>

#ifdef signal_add
  #define signal_add_gtk signal_add
#endif 
#include <event.h>
#undef signal_add
#ifdef signal_add_gtk
  #define signal_add signal_add_gtk
#endif

typedef struct event libevent_event_t;


namespace somanetwork { 
typedef boost::function<void (int fd)> eventCallback_t; 
typedef std::map<int, eventCallback_t> callbackTable_t; 
typedef std::list<eventCallback_t> callbackList_t; 
const int EPOLLMAXCNT = 256; 


class  EventDispatcher
{
 public:
  EventDispatcher(); 
  ~EventDispatcher(); 
  
  // thread safe (can call from other threads)
  void run(); 
  void halt(); 
  void runonce(int maxwait); 

    void dispatchEvent(int fd);
    
  void addEvent(int fd, eventCallback_t cb); 
  void delEvent(int fd); 

  void addTimeout(eventCallback_t); 
  void delTimeout(eventCallback_t); 

 private:
  int epollFD_; 

  bool running_; 
    
  int controlFDw_, controlFDr_; 
  
  callbackTable_t callbackTable_; 
  boost::mutex cbTableMutex_;
  
    
  std::map<int, struct event*> eventTable_; 
  boost::mutex eventTableMutex_;
    
  callbackList_t timeouts_; 
  boost::mutex cbTimeoutsMutex_; 

  void controlEvent(int dummy); 
  
}; 

typedef boost::shared_ptr<EventDispatcher> eventDispatcherPtr_t; 
} 
#endif // EVENTDISPATCHER_H

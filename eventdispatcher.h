#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <utility>
#include <sys/epoll.h>
#include <errno.h>
#include <iostream>

typedef boost::function<void (int fd)> eventCallback_t; 
typedef std::map<int, eventCallback_t> callbackTable_t; 

const int EPOLLMAXCNT = 256; 


class  EventDispatcher
{
 public:
  EventDispatcher(); 
  ~EventDispatcher(); 
  
  // thread safe (can call from other threads)
  void run(); 
  void halt(); 
  void runonce(); 

  void addEvent(int fd, eventCallback_t cb); 
  
  void delEvent(int fd); 
  
 private:
  int epollFD_; 
  bool running_; 
  int controlFDw_, controlFDr_; 
  callbackTable_t callbackTable_; 
  boost::mutex cbTableMutex_;
  
  void controlEvent(int dummy); 
  
}; 

typedef boost::shared_ptr<EventDispatcher> eventDispatcherPtr_t; 

#endif // EVENTDISPATCHER_H

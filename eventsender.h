#ifndef EVENTSENDER_H
#define EVENTSENDER_H

#include <ctime>
#include <iostream>
#include <queue>
#include <map>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> //include all types plus i/o

#include "data/event.h"
#include "data/eventtx.h"
#include "packetreceiver.h"

struct EventTXPending_t
{
  eventtxnonce_t nonce; 
  timeval time; 
  std::vector<char> buffer; 
}; 

typedef std::list<EventTXPending_t *> pendingQueue_t; 

class EventSender : PacketReceiver
{

 public:
  EventSender(int epollfd, std::string somaIP); 
  ~EventSender(); 
  
  void sendEvents(const EventTXList_t & el);
  void handleReceive(); 
  
  void checkPending(); 
  
 private:
  int epollFD_; 
  eventtxnonce_t nonce_; 
  struct epoll_event  ev_; 
  
  int sendSock_; 
  sockaddr_in saServer_; 
  void sendPacket(  EventTXPending_t * etp);

  int responseSock_; 
  pendingQueue_t pendingRespQueue_; 
  boost::mutex appendMutex_;

  
};

#endif // EVENTSENDER_H

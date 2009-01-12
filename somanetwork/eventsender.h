#ifndef SOMANETWORK_EVENTSENDER_H
#define SOMANETWORK_EVENTSENDER_H

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

#include <somanetwork/event.h>
#include <somanetwork/eventtx.h>
#include <somanetwork/packetreceiver.h>
#include <somanetwork/eventdispatcher.h>

namespace somanetwork { 
const int RETXTIME = 1000; // microseconds
const int RETXCNT = 5; // try and send this many times, max

struct EventTXPending_t
{
  eventtxnonce_t nonce; 
  timeval inserttime; 
  timeval sendtime; 
  int txcnt; 
  std::vector<char> buffer; 
}; 

typedef std::list<EventTXPending_t *> pendingQueue_t; 

class EventSender : PacketReceiver
{

 public:
  EventSender(eventDispatcherPtr_t, std::string somaIP); 
  ~EventSender(); 
  
  eventtxnonce_t sendEvents(const EventTXList_t & el);
  
  // internal functions
  void handleReceive(int fd); 
  void handleNewEventIn(int fd); 
  void newResponse(); 
  void newEventIn(); 
  
  
  void checkPending(); 
  eventtxnonce_t getLastSentNonce() { return lastSentNonce_; } ; 

 private:
  eventDispatcherPtr_t pDispatch_; 

  eventtxnonce_t nonce_, lastSentNonce_; 
  
  int sendSock_; 
  int pipeR_;
  int pipeW_; 
  sockaddr_in saServer_; 
  void sendPacket(  EventTXPending_t * etp);
  void sendPendingEvent(); 


  pendingQueue_t eventQueue_; 
  EventTXPending_t * pPendingPacket_; 
  
  boost::mutex appendMutex_;
  
  
};
}
#endif // EVENTSENDER_H

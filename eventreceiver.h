#ifndef DATARECEIVER_H
#define DATARECEIVER_H

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

typedef std::queue<EventPacket_t *> eventPacketQueue_t; 

typedef std::map<eventseq_t, EventPacket_t*> missingPktHash_t;

class EventReceiver
{
  
public:
  EventReceiver(int epollfd, boost::function<void (EventList_t *)> erxp); 
  ~EventReceiver(); 

  int getSocket() { return socket_;}
  //DataReceiverStats getStats(); 

  void handleReceive();   
private:


  void sendReTxReq(eventseq_t seq); 
  //sockaddr_in & sfrom); 
  
  int socket_; 

  int pktCount_; 
  int latestSeq_;
  int dupeCount_; 
  int pendingCount_; 
  int reTxRxCount_; 
  int outOfOrderCount_; 

  boost::function<void (EventList_t *)>  putIn_; 
  int epollFD_; 

  struct epoll_event  ev_; 

  // received queue
  eventPacketQueue_t queue_; 
  
  // missing packet hash
  missingPktHash_t missingPackets_; 

  void updateOutQueue(void); 
  
  boost::mutex statusMutex_;
  
}; 

 
#endif // DATARECEIVER_H

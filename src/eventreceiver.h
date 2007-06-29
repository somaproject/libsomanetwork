#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

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

#include "event.h"
#include "packetreceiver.h"
#include "eventdispatcher.h" 

typedef std::queue<EventPacket_t *> eventPacketQueue_t; 

class EventReceiver : PacketReceiver
{
  typedef std::map<eventseq_t, EventPacket_t*> missingPktHash_t;
  
public:
  EventReceiver(eventDispatcherPtr_t ed, 
		boost::function<void (EventList_t *)> erxp); 
  ~EventReceiver(); 

  int getSocket() { return socket_;}
  //DataReceiverStats getStats(); 

  void handleReceive(int fd);   
private:


  void sendReTxReq(eventseq_t seq, sockaddr_in sfrom); 
  
  int socket_; 

  int pktCount_; 
  int latestSeq_;
  int dupeCount_; 
  int pendingCount_; 
  int reTxRxCount_; 
  int outOfOrderCount_; 

  boost::function<void (EventList_t *)>  putIn_; 
  eventDispatcherPtr_t pDispatch_; 

  // received queue
  eventPacketQueue_t queue_; 
  
  // missing packet hash
  missingPktHash_t missingPackets_; 

  void updateOutQueue(void); 
  
  boost::mutex statusMutex_;
  
}; 

 
#endif // EVENTRECEIVER_H

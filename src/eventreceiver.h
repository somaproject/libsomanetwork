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

#include <somanetwork/event.h>
#include <somanetwork/packetreceiver.h>
#include <somanetwork/eventdispatcher.h>

namespace somanetwork { 
typedef std::queue<pEventPacket_t> eventPacketQueue_t; 

struct EventReceiverStats
{
  unsigned int pktCount; 
  unsigned int latestSeq;
  unsigned int dupeCount; 
  unsigned int pendingCount; 
  unsigned int missingPacketCount;
  unsigned int reTxRxCount; 
  unsigned int outOfOrderCount; 
}; 


class EventReceiver : PacketReceiver
{
  typedef std::map<eventseq_t, pEventPacket_t> missingPktHash_t;
  
public:
  EventReceiver(eventDispatcherPtr_t ed, 
		boost::function<void (pEventPacket_t)> erxp); 
  ~EventReceiver(); 

  int getSocket() { return socket_;}

  void handleReceive(int fd);   
  EventReceiverStats getStats(); 

private:

  void sendReTxReq(eventseq_t seq, sockaddr_in sfrom); 
  
  int socket_; 

  int pktCount_; 
  int latestSeq_;
  int dupeCount_; 
  int pendingCount_; 
  int reTxRxCount_; 
  int outOfOrderCount_; 

  boost::function<void (pEventPacket_t)>  putIn_; 
  eventDispatcherPtr_t pDispatch_; 

  // received queue
  eventPacketQueue_t queue_; 
  
  // missing packet hash
  missingPktHash_t missingPackets_; 

  void updateOutQueue(void); 
  
  boost::mutex statusMutex_;
  
}; 
}

 
#endif // EVENTRECEIVER_H

#ifndef SOMANETWORK_EVENTRECEIVER_H
#define SOMANETWORK_EVENTRECEIVER_H

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
#include <somanetwork/seqpktproto.h>
#include <somanetwork/sockproxy.h>

namespace somanetwork { 
typedef std::queue<pEventPacket_t> eventPacketQueue_t; 

class EventReceiver : PacketReceiver
{
  typedef std::map<eventseq_t, pEventPacket_t> missingPktHash_t;
  
public:
  EventReceiver(eventDispatcherPtr_t ed, 
		pISocketProxy_t sp, 
		boost::function<void (pEventPacket_t)> erxp); 
  ~EventReceiver(); 

  int getSocket() { return socket_;}

  void handleReceive(int fd);   
  SeqPacketProtoStats getStats(); 
  void resetStats(); 
  
private:
  typedef SequentialPacketProtocol<pEventPacket_t> spp_t; 

  void sendReTxReq(eventseq_t seq, sockaddr_in sfrom); 
  static const uint32_t SEQMAX = 0xFFFFFFFF; 

  int socket_; 
  spp_t seqpacketproto_; 

  boost::function<void (pEventPacket_t)>  putIn_; 
  eventDispatcherPtr_t pDispatch_; 
  
  boost::mutex statusMutex_;
  pISocketProxy_t pSockProxy_; 
  
}; 
}

 
#endif // EVENTRECEIVER_H

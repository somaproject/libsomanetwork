#ifndef SOMANETWORK_DATARECEIVER_H
#define SOMANETWORK_DATARECEIVER_H

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

#include <somanetwork/datapacket.h>
#include <somanetwork/packetreceiver.h>
#include <somanetwork/eventdispatcher.h>
#include <somanetwork/seqpktproto.h>
#include <somanetwork/sockproxy.h>

namespace somanetwork { 

int dataPortLookup(int type, int source); 

class DataReceiverStats {
public : 
  datasource_t source; 
  datatype_t type; 
  SeqPacketProtoStats seqprotostats; 

}; 

class DataReceiver : public PacketReceiver
{
  
public:
  DataReceiver(eventDispatcherPtr_t pDispatch, 
	       pISocketProxy_t sockProxy, 
	       int source, datatype_t type,
	       boost::function<void (pDataPacket_t)> rdp); 
  ~DataReceiver(); 

  DataReceiverStats getStats(); 
  void resetStats(); 

  void handleReceive(int fd);   

private:

  typedef SequentialPacketProtocol<pDataPacket_t> spp_t; 

  void sendReTxReq(datasource_t src, datatype_t typ, sequence_t seq,
		   sockaddr_in & sfrom); 

  int socket_; 

  int source_; 
  datatype_t type_; 
  spp_t seqpacketproto_; 
  static const uint32_t SEQMAX = 0xFFFFFFFF; 
  boost::function<void (pDataPacket_t)>  putIn_; 

  // missing packet hash
  
  boost::mutex statusMutex_;
  eventDispatcherPtr_t pDispatch_;    
  pISocketProxy_t pSockProxy_; 

}; 



pDataPacket_t  newDataPacket(boost::array<char, BUFSIZE> buffer) ; 
}

#endif // DATARECEIVER_H

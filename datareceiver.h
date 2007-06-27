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

#include "data/rawdata.h"
#include "packetreceiver.h"

typedef std::queue<DataPacket_t*> rawQueue_t; 


// missing packet type
typedef std::map<sequence_t, 
		   DataPacket_t*> missingPktHash_t;

struct DataReceiverStats
{
  
  int source; 
  int type; 
  int pktCount; 
  int latestSeq;
  int dupeCount; 
  int pendingCount; 
  int missingPacketCount;
  int reTxRxCount; 
  int outOfOrderCount; 
}; 

int dataPortLookup(int type, int source); 

class DataReceiver : public PacketReceiver
{
  
public:
  DataReceiver(int epollfd, int source, datatype_t type,
	       boost::function<void (DataPacket_t *)> rdp); 
  ~DataReceiver(); 
  int getBufferSize(void) 
    {  
      return pktCount_; // rawBuffer_.size();
    }
  int getPktCount() { return pktCount_;} 
  int getLatestSeq() { return latestSeq_;}
  int getDupeCount() { return dupeCount_;}
  int getPendingCount() { return pendingCount_; }
  int getSocket() { return socket_;}
  DataReceiverStats getStats(); 

  void handleReceive();   
private:


  void sendReTxReq(datasource_t src, datatype_t typ, sequence_t seq,
		   sockaddr_in & sfrom); 
  
  int socket_; 

  int source_; 
  datatype_t type_; 
  int pktCount_; 
  int latestSeq_;
  int dupeCount_; 
  int pendingCount_; 
  int reTxRxCount_; 
  int outOfOrderCount_; 
  boost::function<void (DataPacket_t *)>  putIn_; 
  int epollFD_; 
  //ptime firstPacket_, lastPacket_;
  struct epoll_event  ev_; 


  // received queue
  rawQueue_t rawRxQueue_; 
  
  // missing packet hash
  missingPktHash_t missingPackets_; 

  void updateOutQueue(void); 
  
  boost::mutex statusMutex_;
  
}; 



DataPacket_t *  newDataPacket(boost::array<char, BUFSIZE> buffer) ; 
 
#endif // DATARECEIVER_H

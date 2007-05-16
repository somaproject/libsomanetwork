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

#include <boost/date_time/posix_time/posix_time.hpp> //include all types plus i/o

#include <data/rawdata.h>

typedef std::queue<RawData*> rawQueue_t; 


// missing packet type
typedef std::map<sequence_t, 
		   RawData*> missingPktHash_t;

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

class DataReceiver
{
  
public:
  DataReceiver(int epollfd, int source, int type,
	       boost::function<void (RawData *)> rdp); 
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
  int type_; 
  int pktCount_; 
  int latestSeq_;
  int dupeCount_; 
  int pendingCount_; 
  int reTxRxCount_; 
  int outOfOrderCount_; 
  boost::function<void (RawData *)>  putIn_; 
  int epollFD_; 
  //ptime firstPacket_, lastPacket_;
  struct epoll_event  ev_; 


  // received queue
  rawQueue_t rawRxQueue_; 
  
  // missing packet hash
  missingPktHash_t missingPackets_; 

  void updateOutQueue(void); 
  
  // pipe logic
  int readPipe_, writePipe_; 
  
}; 



RawData *  newRawData(boost::array<char, BUFSIZE> buffer) ; 
 
#endif // DATARECEIVER_H

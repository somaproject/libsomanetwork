#ifndef DATARECEIVER_H
#define DATARECEIVER_H

#include <ctime>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> //include all types plus i/o
using namespace boost::posix_time;
const int BUFSIZE = 1024; 
const int HDRLEN = 6;

typedef unsigned char datasource_t; 
typedef unsigned char datatype_t; 
typedef unsigned int sequence_t; 

struct RawData
{
  sequence_t seq; 
  unsigned char src; 
  unsigned char typ; 
  bool missing; 
  boost::array<char, BUFSIZE - HDRLEN> body;
}; 

typedef std::list<RawData*> rawQueue_t; 


// missing packet type
typedef std::map<sequence_t, 
		   rawQueue_t::iterator> missingIterHash_t;


int dataPortLookup(int type, int source); 

class DataReceiver
{
  
public:
  DataReceiver(asio::io_service& io_service, 
	       int source, int type,
	       boost::function<void (RawData *)> rdp); 
  ~DataReceiver(); 
  int getBufferSize(void) 
    {  
      return pktCount_; // rawBuffer_.size();
    }
  

private:
  void startReceive(); 
  void handleReceive(const asio::error_code& error,
		   std::size_t /*bytes_transferred*/); 
  void sendReTxReq(datasource_t src, datatype_t typ, sequence_t seq); 
  void handleSend(char * message,
		  const asio::error_code& /*error*/,
		  std::size_t /*bytes_transferred*/);

  
  int source_; 
  int type_; 
  int pktCount_; 
  int latestSeq_; 

  boost::function<void (RawData *)>  putIn_; 
  
  //ptime firstPacket_, lastPacket_;

  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint remote_endpoint_;
  boost::array<char, BUFSIZE> recv_buffer_;

  int dupeCount_; 

  // received queue
  rawQueue_t rawRxQueue_; 
  
  // missing packet hash
  missingIterHash_t missingPacketIters_; 

  void updateOutQueue(void); 
  
  // pipe logic
  int readPipe_, writePipe_; 
  
}; 



RawData *  newRawData(boost::array<char, BUFSIZE> buffer) ; 
 
#endif // DATARECEIVER_H

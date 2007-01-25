#include "datareceiver.h"
#include <iostream>
#include <arpa/inet.h>
using asio::ip::udp;


int dataPortLookup(int type, int source) {
  return 4000  + type*64 + source;  
}

RawData * newRawData(boost::array<char, BUFSIZE> buffer) 
{
  RawData * prd = new RawData; 
//   std::cout << (int) buffer[0] << ' '
// 	    << (int) buffer[1] << ' '
// 	    << (int) buffer[2] << ' '
// 	    << (int) buffer[3] << std::endl; 
    
  prd->seq = ntohl(*((int *) &buffer[0])); 
  prd->src = buffer[4]; 
  prd->typ = buffer[5]; 
  prd->missing = false; 

  for(int i = HDRLEN; i < BUFSIZE; i++) {
    prd->body[i - HDRLEN] = buffer[i]; 
  }

  return prd; 
}

DataReceiver::DataReceiver(asio::io_service& io_service, 
			   int source, int type, 
			   boost::function<void (RawData *)> rdp)
  : source_ (source), 
    type_ (type), 
    pktCount_(0),
    socket_(io_service, 
	    udp::endpoint(udp::v4(), dataPortLookup(type, source))),
    putIn_(rdp)
{
  startReceive(); 


}


DataReceiver::~DataReceiver()
{
}

void DataReceiver::startReceive()
{
  socket_.async_receive_from(asio::buffer(recv_buffer_, BUFSIZE),
			     remote_endpoint_,
			     boost::bind(&DataReceiver::handleReceive, 
					 this,
					 asio::placeholders::error,
					 asio::placeholders::bytes_transferred));
}


void DataReceiver::handleReceive(const asio::error_code& error,
				 std::size_t bytes_transferred)
{

  if (!error )
    {

      RawData * prd = newRawData(recv_buffer_); 

      if ( pktCount_ == 0 or prd->seq == latestSeq_ + 1)
	{
	  // this is the next packet, append
	  rawRxQueue_.push_back(prd); 
	  latestSeq_ = prd->seq; 

	  pktCount_++; 
	} 
      else if (prd->seq > latestSeq_ + 1)
	{
	  // we're missing a packet; add in blanks with "missing" set
	  
	  for (int i = 0; i < (prd->seq - (latestSeq_ +1)); i++) 
	    {
	      RawData * missingPkt = new RawData; 
	      sequence_t missingSeq =  latestSeq_ + i; 
	      missingPkt-> seq = missingSeq; 
	      missingPkt->missing = false; 
	      rawRxQueue_.push_back(missingPkt); 
	      latestSeq_++; 
	      // now add iterators
	      missingPacketIters_[missingSeq] = rawRxQueue_.end(); 
	      
	      // now request a retx 
	      // NEED TO IMPLEMENT

	    }
	  
	  // then add this after that
	  rawRxQueue_.push_back(prd); 
	  latestSeq_++;
	  pktCount_++; 
	  
	} 
      else 
	{
	  // it's in the past, which means it's either a dupe 
	  // or on our missing list
	  
	  // check if it's a missing packet
	  missingIterHash_t::iterator m = missingPacketIters_.find(prd->seq); 
	  if (m == missingPacketIters_.end() ) 
	    {
	    // this was a duplicate packet; ignore
	    dupeCount_++; 
	    
	    } 
	  else 
	    { 
	      // get the iterator 
	      rawQueue_t::iterator pkt = (*m).second; 
	      missingPacketIters_.erase(m); 
	      
	      // now, delete the (empty) packet that the pointer points to
	      delete *pkt; 
	      
	      // set the iterator to be the new just-rx'd packet
	      *pkt = prd;
	      
	      pktCount_++; 
	      
	    }
	  
	  
	}
      
      
      // push packets out via output queue
      updateOutQueue(); 
      startReceive(); // call our receive function again? 
      
    }
}

void DataReceiver::updateOutQueue()
{
  /* Update output queue pushes new data into the output queue and writes 
     to the output pipe until 
     
     
  */ 
  
  int updateCount = 0;  // the number of new packets we've added to the queue
  // extract out
  while (not rawRxQueue_.empty() and 
	 (*rawRxQueue_.front()).missing == false) {
    
    putIn_(rawRxQueue_.front()); 
    
    rawRxQueue_.pop_front(); 
    
    updateCount++ ; 
    
  }
  
}



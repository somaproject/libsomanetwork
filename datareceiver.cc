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
//    std::cout << (int) buffer[0] << ' '
//  	    << (int) buffer[1] << ' '
//  	    << (int) buffer[2] << ' '
//  	    << (int) buffer[3] << ' '
//  	    << (int) buffer[4] << ' '
//  	    << (int) buffer[5] << std::endl; 
    
  prd->seq = ntohl(*((int *) &buffer[0])); 
  prd->typ = buffer[4]; 
  prd->src = buffer[5]; 
  prd->missing = false; 

  for(int i = HDRLEN; i < BUFSIZE; i++) {
    //prd->body[i - HDRLEN] = buffer[i]; 
  }

  return prd; 
}

DataReceiver::DataReceiver(asio::io_service& io_service, 
			   int source, int type, 
			   boost::function<void (RawData *)> rdp)
  : source_ (source), 
    type_ (type), 
    pktCount_(0),
    latestSeq_(0), 
    dupeCount_(0), 
    pendingCount_(0), 
    reTxRxCount_(0), 
    outOfOrderCount_(0),
    socket_(io_service, 
	    udp::endpoint(udp::v4(), dataPortLookup(type, source))),
    putIn_(rdp)
{

//   asio::socket_base::receive_buffer_size optionset(300000);
//   socket_.set_option(optionset); 

//   asio::socket_base::receive_buffer_size option;
  
//   socket_.get_option(option);
//   int size = option.value(); 
//  std::cout << "The socket buffer size is " << size << std::endl; 

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

void DataReceiver::sendReTxReq(datasource_t src, datatype_t typ, unsigned
			       int seq)
{

  char * retxbuf =  new char[6]; 
  retxbuf[0] = typ; 
  retxbuf[1] = src; 
  unsigned int seqn = htonl(seq); 
  
  memcpy(&retxbuf[2], &seqn, 4); 
  
  udp::endpoint retxep = remote_endpoint_;
  retxep.port(4400); 

//   std::cout << "I am  " << source_ << " !" 
// 		    << " sending retx for  "
// 		    << (int) src << " | " 
// 		    << (int) typ << " | " << seq << std::endl; 
	  
  
  socket_.async_send_to(asio::buffer(retxbuf, 6), 
			retxep,
			boost::bind(&DataReceiver::handleSend, 
				    this, 
				    retxbuf,
				    asio::placeholders::error,
				    asio::placeholders::bytes_transferred));

}

void DataReceiver::handleSend(char * message,
			      const asio::error_code& /*error*/,
			      std::size_t /*bytes_transferred*/)
{
  delete message; 
}

void DataReceiver::handleReceive(const asio::error_code& error,
				 std::size_t bytes_transferred)
{

  if (!error )
    {

      RawData * prd = newRawData(recv_buffer_); 
      if (prd->src != source_ or prd->typ != type_) {
	std::cerr  << "Error receiving packet " 
		   << (int) prd->src << " != " << source_  << " or " 
		   << (int) prd->typ << " != " << type_  << std::endl; 
      }

      if ( pktCount_ == 0 or prd->seq == latestSeq_ + 1)
	{
	  // this is the next packet, append
	  rawRxQueue_.push_back(prd); 
	  pendingCount_++; 

	  latestSeq_ = prd->seq; 

	  pktCount_++; 

	} 
      else if (prd->seq > latestSeq_ + 1)
	{
	  // we're missing a packet; add in blanks with "missing" set
	  std::cout << "I am " << source_ << " and I received seq=" 
		    << prd->seq << " when I was expecting " << latestSeq_ + 1
		    << std::endl;
	  sequence_t missingSeq;
	  for (int i = 0; i < (prd->seq - (latestSeq_ +1)); i++) 
	    {
	      RawData * missingPkt = new RawData; 
	      missingSeq =  latestSeq_ + i + 1; 
	      missingPkt-> seq = missingSeq; 
	      missingPkt->typ = type_; 
	      missingPkt->src = source_; 
	      missingPkt->missing = true; 
	      rawRxQueue_.push_back(missingPkt); 
	      pendingCount_++; 

	       
	      // now add missing packets
	      missingPackets_[missingSeq] = missingPkt; 
		
	      // now request a retx 
	      sendReTxReq(prd->src,  prd->typ, missingSeq); 

	    }
	  
	  // then add this after that
	  rawRxQueue_.push_back(prd); 
	  pendingCount_++; 
	  std::cout << "After missing some, latestseq_ = " << latestSeq_ 
		    << " and prd->seq=" << prd->seq 
		    << " and I most recently pushed " << missingSeq << std::endl; 
	  latestSeq_ = prd->seq;
	  pktCount_++; 
	  
	} 
      else 
	{
	  // it's in the past, which means it's either a dupe 
	  // or on our missing list
	  std::cout << "I am " << source_ << " !" 
		    << " packet received in past for  "
		    << (int) prd->src << " | " 
		    << (int) prd->typ << " | " << prd->seq << std::endl; 
	  
	  // check if it's a missing packet
	  missingPktHash_t::iterator m = missingPackets_.find(prd->seq); 
	  if (m == missingPackets_.end() ) 
	    {
	    // this was a duplicate packet; ignore
	    dupeCount_++; 
	    
	    } 
	  else 
	    { 
	      // get the iterator 
	      RawData* pkt = (*m).second; 

	      // copy the received packet into the one that's currently in the retx buffer

	      *pkt = *prd; 
	      missingPackets_.erase(m); 

	      delete prd; 

	      pktCount_++; 
	      if (recv_buffer_[6] != 0) {
		reTxRxCount_++;
	      } else {
		outOfOrderCount_++; 
	      }
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

  if ((*rawRxQueue_.front()).missing == true) {
    if (rawRxQueue_.size() > 10) {
//       RawData * rdp = rawRxQueue_.front(); 
//       sendReTxReq(rdp->src,  rdp->typ, rdp->seq); 
//       std::cout << "I am " << source_ << " sending retx req for " 
// 		<< (int) rdp->src << " | " 
// 		<< (int) rdp->typ << " | " << rdp->seq << std::endl; 
      
    }
  }
  while (not rawRxQueue_.empty() and 
	 (*rawRxQueue_.front()).missing == false) {
    

    RawData* rdp = rawRxQueue_.front(); 
 

    putIn_(rdp); 
    
    rawRxQueue_.pop_front(); 
    pendingCount_--; 	
   
    updateCount++ ; 
    
  }
  
}

DataReceiverStats DataReceiver::getStats()
{
  DataReceiverStats st; 
  st.source = source_; 
  st.type = type_; 
  st.pktCount = pktCount_; 
  st.latestSeq = latestSeq_; 
  st.dupeCount = dupeCount_; 
  st.pendingCount = pendingCount_;
  
  st.missingPacketCount = missingPackets_.size(); 
  st.reTxRxCount = 	      reTxRxCount_; 
  st.outOfOrderCount = outOfOrderCount_; 
  return DataReceiverStats(st); 
}

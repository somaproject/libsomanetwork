#include <iostream>
#include <arpa/inet.h>
#include "datareceiver.h"


DataReceiver::DataReceiver(int epollfd, int source, datatype_t type, 
			   boost::function<void (RawData *)> rdp)
  : source_ (source), 
    type_ (type), 
    pktCount_(0),
    latestSeq_(0), 
    dupeCount_(0), 
    pendingCount_(0), 
    reTxRxCount_(0), 
    outOfOrderCount_(0),
    putIn_(rdp), 
    epollFD_(epollfd)
{

  struct sockaddr_in si_me, si_other;
  int  slen=sizeof(si_other);
    
  socket_ = socket(AF_INET, SOCK_DGRAM, 17); 
  if (socket_ < 0) {
    throw std::runtime_error("could not create socket"); 

  }

  memset((char *) &si_me, sizeof(si_me), 0);

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(4000 + type_*64 + source_ ); 

  si_me.sin_addr.s_addr = INADDR_ANY; 
  
  int optval = 1; 

  // confiugre socket for reuse
  optval = 1; 
  int res = setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, 
	     &optval, sizeof (optval)); 
  if (res < 0) {
    throw std::runtime_error("error settng socket to reuse"); 
  }

  optval = 500000; 
  res = setsockopt (socket_, SOL_SOCKET, SO_RCVBUF, 
		    (const void *) &optval, sizeof(optval)); 
  if (res < 0) {
    throw std::runtime_error("error settng receive buffer size"); 

  }

  res =  bind(socket_, (sockaddr*)&si_me, sizeof(si_me)); 
  if (res < 0) {
    throw std::runtime_error("error binding socket"); 
  }
    
  // try adding to epoll
  ev_.events = EPOLLIN; 
  ev_.data.fd = socket_;
  ev_.data.ptr = this; // store self!
  res = epoll_ctl(epollFD_, EPOLL_CTL_ADD, socket_, &ev_); 

}


DataReceiver::~DataReceiver()
{
  
  // remove from epoll
  int res = epoll_ctl(epollFD_, EPOLL_CTL_DEL, socket_, NULL); 
  
    close(socket_); 

}


void DataReceiver::sendReTxReq(datasource_t src, datatype_t typ, unsigned
			       int seq,  sockaddr_in & sfrom)
{

  char * retxbuf =  new char[6]; 
  retxbuf[0] = typ; 
  retxbuf[1] = src; 
  unsigned int seqn = htonl(seq); 
  memcpy(&retxbuf[2], &seqn, 4); 

  sfrom.sin_port = htons(4400); 
  sendto(socket_, &retxbuf[0], 6, 0, (sockaddr*)&sfrom , sizeof(sfrom)); 

}


void DataReceiver::handleReceive()
{


  boost::array<char, BUFSIZE> recvbuffer; 
  sockaddr_in sfrom; 
  socklen_t fromlen = sizeof(sfrom); 

  int error = recvfrom(socket_, &recvbuffer[0], BUFSIZE, 
		   0, (sockaddr*)&sfrom, &fromlen); 
      
  if ( error == -1 )
    { 
      std::cerr << "error in recvfrom" << std::endl; 
    } else
    {

      RawData * prd = newRawData(recvbuffer); 

      if (prd->src != source_ or prd->typ != type_) {
	std::cerr  << "Error receiving packet " 
		   << (int) prd->src << " != " << source_  << " or " 
		   << (int) prd->typ << " != " << type_  << std::endl; 
      }

      if ( pktCount_ == 0 or prd->seq == latestSeq_ + 1)
	{
	  // this is the next packet, append
	  rawRxQueue_.push(prd); 
	  pendingCount_++; 

	  latestSeq_ = prd->seq; 

	  pktCount_++; 

	} 
      else if (prd->seq > latestSeq_ + 1)
	{
	  // we're missing a packet; add in blanks with "missing" set

	  sequence_t missingSeq;
	  for (int i = 0; i < (prd->seq - (latestSeq_ +1)); i++) 
	    {
	      RawData * missingPkt = new RawData; 
	      missingSeq =  latestSeq_ + i + 1; 
	      missingPkt-> seq = missingSeq; 
	      missingPkt->typ = type_; 
	      missingPkt->src = source_; 
	      missingPkt->missing = true; 
	      rawRxQueue_.push(missingPkt); 
	      pendingCount_++; 

	       
	      // now add missing packets
	      missingPackets_[missingSeq] = missingPkt; 
		
	      // now request a retx 
	      sendReTxReq(prd->src,  prd->typ, missingSeq, sfrom); 

	    }
	  
	  // then add this after that
	  rawRxQueue_.push(prd); 
	  pendingCount_++; 

	  latestSeq_ = prd->seq;
	  pktCount_++; 
	  
	} 
      else 
	{
	  // it's in the past, which means it's either a dupe 
	  // or on our missing list

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
	      if (recvbuffer[6] != 0) {
		reTxRxCount_++;
	      } else {
		outOfOrderCount_++; 
	      }
	    }
	  
	  
	}
      
      
      // push packets out via output queue
      updateOutQueue(); 
      
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
      
    }
  }
  while (not rawRxQueue_.empty() and 
	 (*rawRxQueue_.front()).missing == false) 
    {
    
    
    RawData* rdp = rawRxQueue_.front(); 
    
    
    putIn_(rdp); 
    
    rawRxQueue_.pop(); 
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

#include <iostream>
#include <arpa/inet.h>
#include "eventreceiver.h"
#include "ports.h"

EventReceiver::EventReceiver(eventDispatcherPtr_t ed, 
			     boost::function<void (EventList_t *)> erxp)
  : pktCount_(0),
    latestSeq_(0), 
    dupeCount_(0), 
    pendingCount_(0), 
    reTxRxCount_(0), 
    outOfOrderCount_(0),
    putIn_(erxp), 
    pDispatch_(ed)
{

  struct sockaddr_in si_me, si_other;
  int  slen=sizeof(si_other);
    
  socket_ = socket(AF_INET, SOCK_DGRAM, 17); 
  if (socket_ < 0) {
    throw std::runtime_error("could not create socket"); 

  }
  
  memset((char *) &si_me, sizeof(si_me), 0);

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(EVENTRXPORT); 

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
    
  // try adding to dispatcher
  pDispatch_->addEvent(socket_, 
		       boost::bind(std::mem_fun(&EventReceiver::handleReceive),
				   this, _1)); 
  
}


EventReceiver::~EventReceiver()
{
  
  pDispatch_->delEvent(socket_); 
  close(socket_); 

}


void EventReceiver::sendReTxReq(eventseq_t seq, sockaddr_in sfrom)
{

  char * retxbuf =  new char[4]; 
  unsigned int seqn = htonl(seq); 
  memcpy(&retxbuf[0], &seqn, 4); 

  sfrom.sin_port = htons(EVENTRXRETXPORT); 
  sendto(socket_, &retxbuf[0], 4, 0, (sockaddr*)&sfrom , sizeof(sfrom)); 

}


void EventReceiver::handleReceive(int fd)
{

  boost::mutex::scoped_lock lock( statusMutex_ );

  boost::array<char, EBUFSIZE> recvbuffer; 
  sockaddr_in sfrom; 
  socklen_t fromlen = sizeof(sfrom); 

  size_t len = recvfrom(socket_, &recvbuffer[0], EBUFSIZE, 
		   0, (sockaddr*)&sfrom, &fromlen); 
      
  if ( len == -1 )
    { 
      std::cerr << "error in recvfrom" << std::endl; 
    } else
    {

      // do we always extract out the events? 
      EventPacket_t * pEventPacket = newEventPacket(recvbuffer, len); 
      
      if ( pktCount_ == 0 or pEventPacket->seq == latestSeq_ + 1)
	{
	  // this is the next packet, append
	  queue_.push(pEventPacket); 
	  pendingCount_++; 

	  latestSeq_ = pEventPacket->seq; 

	  pktCount_++; 

	} 
      else if (pEventPacket->seq > latestSeq_ + 1)
	{
	  // we're missing a packet; add in blanks with "missing" set
	  std::cout << "We're missing a packet; we got seq="
		    <<  pEventPacket->seq << " instead of " 
		    << latestSeq_ + 1  
		    << std::endl; 

	  eventseq_t missingSeq;
	  for (int i = 0; i < (pEventPacket->seq - (latestSeq_ +1)); i++) 
	    {
	      EventPacket_t * missingPkt = new EventPacket_t; 
	      missingSeq =  latestSeq_ + i + 1; 
	      missingPkt->seq = missingSeq; 
	      missingPkt->missing = true; 
	      queue_.push(missingPkt); 
	      pendingCount_++; 

	       
	      // now add missing packets
	      missingPackets_[missingSeq] = missingPkt; 
	      
	      // now request a retx 
	      sendReTxReq(missingSeq, sfrom); 

	    }
	  
	  // then add this after that
	  queue_.push(pEventPacket); 
	  pendingCount_++; 

	  latestSeq_ = pEventPacket->seq;
	  pktCount_++; 
	  
	} 
      else 
	{
	  // it's in the past, which means it's either a dupe 
	  // or on our missing list

	  // check if it's a missing packet
	  missingPktHash_t::iterator m 
	    = missingPackets_.find(pEventPacket->seq); 

	  if (m == missingPackets_.end() ) 
	    {
	    // this was a duplicate packet; ignore
	    dupeCount_++; 
	    
	    } 
	  else 
	    { 
	      // get the iterator 
	     EventPacket_t * pkt = (*m).second; 

	      // copy the received packet into the one 
	     // that's currently in the retx buffer

	     *pkt = *pEventPacket; 
	     missingPackets_.erase(m); 
	     
	     delete pEventPacket; 
	     
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

void EventReceiver::updateOutQueue()
{
  /* Update output queue pushes new data into the output queue and writes 
     to the output pipe until 
     
     
  */ 
  
  int updateCount = 0;  // the number of new packets we've added to the queue
  // extract out

  if ((*queue_.front()).missing == true) {
    if (queue_.size() > 10) {
      //       RawData * rdp = rawRxQueue_.front(); 
      //       sendReTxReq(rdp->src,  rdp->typ, rdp->seq); 
      
    }
  }
  while (not queue_.empty() and 
	 (*queue_.front()).missing == false) 
    {
    
      
      EventPacket_t * pep = queue_.front(); 
    
      
      putIn_(pep->events); 
      
      queue_.pop(); 
      
      delete pep; // we've passed on the internal events

      pendingCount_--; 	
      
      updateCount++ ; 
      
    }
  
}

// EventReceiverStats EventReceiver::getStats()
// {
//   // This is now thread safe thanks to the mutex

//   boost::mutex::scoped_lock lock( statusMutex_ );
//   EventReceiverStats st; 
//   st.source = source_; 
//   st.type = type_; 
//   st.pktCount = pktCount_; 
//   st.latestSeq = latestSeq_; 
//   st.dupeCount = dupeCount_; 
//   st.pendingCount = pendingCount_;
  
//   st.missingPacketCount = missingPackets_.size(); 
//   st.reTxRxCount = 	      reTxRxCount_; 
//   st.outOfOrderCount = outOfOrderCount_; 
//   return EventReceiverStats(st); 
// }

#include <iostream>
#include <arpa/inet.h>
#include "eventreceiver.h"
#include "ports.h"
#include "logging.h"

namespace somanetwork {

EventReceiver::EventReceiver(eventDispatcherPtr_t ed, 
			     pISocketProxy_t sp, 
			     boost::function<void (pEventPacket_t)> erxp)
  : seqpacketproto_(SEQMAX), 
    putIn_(erxp), 
    pDispatch_(ed), 
    pSockProxy_(sp),
    enabled_(false)
{
  socket_ = pSockProxy_->createEventRXSocket(); 
    
  // try adding to dispatcher
  pDispatch_->addEvent(socket_, 
		       boost::bind(std::mem_fun(&EventReceiver::handleReceive),
				   this, _1)); 
  
}


EventReceiver::~EventReceiver()
{
  
  pDispatch_->delEvent(socket_); 
  pSockProxy_->closeSocket(socket_); 

}

void EventReceiver::setEnabled(bool state)
{
  enabled_ = state; 
}

void EventReceiver::sendReTxReq(eventseq_t seq, sockaddr_in sfrom)
{
  L_(info) << "EventReceiver:" 
	   << "requesting retranmission "
	   << " sequence = " << seq; 


  char * retxbuf =  new char[4]; 
  unsigned int seqn = htonl(seq); 
  memcpy(&retxbuf[0], &seqn, 4); 

  sendto(socket_, &retxbuf[0], 4, 0, 
	 pSockProxy_->getEventReTxReqSockAddr(), 
	 pSockProxy_->getEventReTxReqSockAddrLen()); 
  delete[] retxbuf; 

}


void EventReceiver::handleReceive(int fd)
{
  L_(debug) << "EventReceiver::handle receive"; 
  boost::mutex::scoped_lock lock( statusMutex_ );

  boost::array<char, EBUFSIZE> recvbuffer; 
  sockaddr_in sfrom; 
  socklen_t fromlen = sizeof(sfrom); 

  size_t len = recvfrom(socket_, &recvbuffer[0], EBUFSIZE, 
		   0, (sockaddr*)&sfrom, &fromlen); 
      
  if ( len == -1 )
    { 
      L_(warning) << "EventReceiver error in ReceiveFrom"; 
    } else
    {

      // do we always extract out the events? 
      pEventPacket_t pEventPacket = newEventPacket(recvbuffer, len); 
      seqpacketproto_.addPacket(pEventPacket, pEventPacket->seq); 
	    
      SequentialPacketProtocol<pEventPacket_t>::outqueue_t out = 
	seqpacketproto_.getCompletedPackets(); 
      
      // check for retransmission
      std::list<seqid_t> retxes = seqpacketproto_.getRetransmitRequests(); 
      for (std::list<seqid_t>::iterator ri = retxes.begin(); ri != retxes.end(); 
	   ri++) 
	{
	  sendReTxReq(*ri, sfrom); 
	}

      // commit the outputs

      SequentialPacketProtocol<pEventPacket_t>::outqueue_t::iterator i; 
      for (i = out.begin(); i != out.end(); i++) {
	// FIXME: enable/disable should actually control the socket!
	if (enabled_)
	  putIn_(*i); 	
	
      }
      

    } 
}

SeqPacketProtoStats EventReceiver::getStats()
 {
   // This is now thread safe thanks to the mutex

  boost::mutex::scoped_lock lock( statusMutex_ );
  return seqpacketproto_.getStats(); 
}

void EventReceiver::resetStats()
 {
   // This is now thread safe thanks to the mutex

  boost::mutex::scoped_lock lock( statusMutex_ );
  return seqpacketproto_.resetStats(); 
}

}

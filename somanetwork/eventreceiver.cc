#include <iostream>
#include <arpa/inet.h>
#include "eventreceiver.h"
#include "ports.h"

namespace somanetwork {

EventReceiver::EventReceiver(eventDispatcherPtr_t ed, 
			     boost::function<void (pEventPacket_t)> erxp)
  : seqpacketproto_(SEQMAX), 
    putIn_(erxp), 
    pDispatch_(ed)
{

  struct sockaddr_in si_me, si_other;
  int  slen=sizeof(si_other);
    
  socket_ = socket(AF_INET, SOCK_DGRAM, 17); 
  if (socket_ < 0) {
    throw std::runtime_error("could not create socket"); 

  }
  
  bzero((char *) &si_me, sizeof(si_me));

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

  optval = 4000000; 
  res = setsockopt (socket_, SOL_SOCKET, SO_RCVBUF, 
		    (const void *) &optval, sizeof(optval)); 
  if (res < 0) {
    throw std::runtime_error("error settng receive buffer size"); 
  }

  socklen_t optlen;   
  res = getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, 
		   (void *) &optval, &optlen); 

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

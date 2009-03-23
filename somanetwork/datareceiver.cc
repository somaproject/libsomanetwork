#include <iostream>
#include <arpa/inet.h>
#include "datareceiver.h"
#include "eventdispatcher.h"
#include "ports.h"

namespace somanetwork { 
DataReceiver::DataReceiver(eventDispatcherPtr_t dispatch, int source, datatype_t type, 
			   boost::function<void (pDataPacket_t)> rdp)
  : source_ (source), 
    type_ (type),
    seqpacketproto_(SEQMAX), 
    putIn_(rdp), 
    pDispatch_(dispatch)
{


  struct sockaddr_in si_me, si_other;
  int  slen=sizeof(si_other);
    
  socket_ = socket(AF_INET, SOCK_DGRAM, 17); 
  if (socket_ < 0) {
    throw std::runtime_error("could not create socket"); 

  }

  bzero((char *) &si_me, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(dataPortLookup(type_, source_));

  si_me.sin_addr.s_addr = INADDR_ANY; 
  
  int optval = 1; 

  // confiugre socket for reuse
  optval = 1; 
  int res = setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, 
	     &optval, sizeof (optval)); 
  if (res < 0) {
    throw std::runtime_error("error setting socket to reuse"); 
  }

  optval = 1; 
  res = setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, 
	     &optval, sizeof (optval)); 
  if (res < 0) {
    throw std::runtime_error("error setting the broadcast bit"); 
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
    
  // configure the RX dispatch
  pDispatch_->addEvent(socket_, 
		       boost::bind(std::mem_fun(&DataReceiver::handleReceive),
				   this, _1)); 
  
}


DataReceiver::~DataReceiver()
{
  
  // remove from epoll
  pDispatch_->delEvent(socket_); 
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

  sfrom.sin_port = htons(DATARETXPORT); 
  sendto(socket_, &retxbuf[0], 6, 0, (sockaddr*)&sfrom , sizeof(sfrom)); 

}


void DataReceiver::handleReceive(int fd)
{
  boost::mutex::scoped_lock lock( statusMutex_ );

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

      pDataPacket_t prd = newDataPacket(recvbuffer); 

      if (prd->src != source_ or prd->typ != type_) {
	std::cerr  << "Incorrect packet header " << std::endl  
		   << "either pkt source " << (int) prd->src 
		   << " != datarx src " << source_  << " or pkt typ " 
		   << (int) prd->typ << " != datarx typ " << type_  << std::endl; 
      }
      seqpacketproto_.addPacket(prd, prd->seq); 

      SequentialPacketProtocol<pDataPacket_t>::outqueue_t out = 
	seqpacketproto_.getCompletedPackets(); 

      // check for retransmission
      std::list<seqid_t> retxes = seqpacketproto_.getRetransmitRequests(); 
      for (std::list<seqid_t>::iterator ri = retxes.begin(); ri != retxes.end(); 
	   ri++) 
	{
	  sendReTxReq(source_, type_, *ri, sfrom); 
	}

      // commit the outputs

      SequentialPacketProtocol<pDataPacket_t>::outqueue_t::iterator i; 
      for (i = out.begin(); i != out.end(); i++) {
	putIn_(*i); 	
      }
      
    } 
}


SeqPacketProtoStats DataReceiver::getStats()
{
  //This is now thread safe thanks to the mutex

  boost::mutex::scoped_lock lock( statusMutex_ );
  return seqpacketproto_.getStats(); 

}

void DataReceiver::resetStats()
{
  boost::mutex::scoped_lock lock( statusMutex_ );
  seqpacketproto_.resetStats(); 
    
}

}

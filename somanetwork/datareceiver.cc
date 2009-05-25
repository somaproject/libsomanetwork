#include <iostream>
#include <arpa/inet.h>
#include "datareceiver.h"
#include "eventdispatcher.h"
#include "ports.h"

namespace somanetwork { 
DataReceiver::DataReceiver(eventDispatcherPtr_t dispatch, 
			   pISocketProxy_t sockProxy, 
			   int source, datatype_t type, 
			   boost::function<void (pDataPacket_t)> rdp)
  : source_ (source), 
    type_ (type),
    seqpacketproto_(SEQMAX), 
    putIn_(rdp), 
    pDispatch_(dispatch), 
    pSockProxy_(sockProxy)
{


  socket_ = pSockProxy_->createDataSocket(source, type); 

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


  sendto(socket_, &retxbuf[0], 6, 0, 
	 pSockProxy_->getDataReTxReqSockAddr(), 
	 pSockProxy_->getDataReTxReqSockAddrLen()); 

  delete[] retxbuf; 

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


DataReceiverStats DataReceiver::getStats()
{
  //This is now thread safe thanks to the mutex

  boost::mutex::scoped_lock lock( statusMutex_ );
  DataReceiverStats s; 
  s.seqprotostats = seqpacketproto_.getStats(); 
  s.source = source_; 
  s.type = type_; 

}

void DataReceiver::resetStats()
{
  boost::mutex::scoped_lock lock( statusMutex_ );
  seqpacketproto_.resetStats(); 
    
}

}

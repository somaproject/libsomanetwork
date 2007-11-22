#include <iostream>
#include <arpa/inet.h>
#include "datareceiver.h"
#include "eventdispatcher.h"
#include "ports.h"

DataReceiver::DataReceiver(eventDispatcherPtr_t dispatch, int source, datatype_t type, 
			   boost::function<void (pDataPacket_t)> rdp)
  : source_ (source), 
    type_ (type), 
    pktCount_(0),
    latestSeq_(0), 
    dupeCount_(0), 
    pendingCount_(0), 
    reTxRxCount_(0), 
    outOfOrderCount_(0),
    putIn_(rdp), 
    pDispatch_(dispatch)
{

  struct sockaddr_in si_me, si_other;
  int  slen=sizeof(si_other);
    
  socket_ = socket(AF_INET, SOCK_DGRAM, 17); 
  if (socket_ < 0) {
    throw std::runtime_error("could not create socket"); 

  }

  memset((char *) &si_me, sizeof(si_me), 0);

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
  
  std::cout << "DataReceiver created with src=" 
	    << (int)source << " typ=" << (int)type << std::endl; 

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
	      pDataPacket_t missingPkt(new DataPacket_t); 
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
	      sendReTxReq(prd->src,  prd->typ,
			  missingSeq, sfrom); 

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
	      pDataPacket_t pkt((*m).second); 

	      // copy the received packet into the one that's currently in the retx buffer

	      *pkt = *prd; 
	      missingPackets_.erase(m); 
	      
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

//   if ((*rawRxQueue_.front()).missing == true) {
//     if (rawRxQueue_.size() > 10) {
//       DataPacket_t * rdp = rawRxQueue_.front(); 
//       //sendReTxReq(rdp->src,  rdp->typ, rdp->seq); 
//       // we should figure out something smart to do here, but right
//       // now there's nothing

//     }
//   }

  while (not rawRxQueue_.empty() and 
	 rawRxQueue_.front()->missing == false) 
    {
      
      pDataPacket_t rdp(rawRxQueue_.front()); 
      
      putIn_(rdp); 
      rawRxQueue_.pop(); 
      pendingCount_--; 	
      
      updateCount++ ; 
      
    }
  
}

DataReceiverStats DataReceiver::getStats()
{
  //This is now thread safe thanks to the mutex

  boost::mutex::scoped_lock lock( statusMutex_ );
  
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

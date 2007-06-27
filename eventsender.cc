#include "eventsender.h"


EventSender::EventSender(int epollfd, std::string somaIP) :
  epollFD_(epollfd),
  nonce_(0)

{


  ////////////////////////////////////////
  // configure the transmit socket
  ////////////////////////////////////////

  sendSock_ = socket(AF_INET, SOCK_DGRAM, 17); 
  if (sendSock_ < 0) {
    throw std::runtime_error("could not create transmit socket"); 
  }
  
  memset(&saServer_, sizeof(saServer_), 0); 
  saServer_.sin_family = AF_INET; 
  saServer_.sin_port = htons(5000);  /// remote port 5000? 
  inet_aton(somaIP.c_str(), &saServer_.sin_addr); 
  

  
  ///////////////////////////////////////
  // bind to the response socket
  ////////////////////////////////////////
  struct sockaddr_in si_me, si_other;
  int  slen=sizeof(si_other);

  responseSock_ = socket(AF_INET, SOCK_DGRAM, 17); 
  if (responseSock_ < 0) {
    throw std::runtime_error("could not create socket"); 

  }

  memset((char *) &si_me, sizeof(si_me), 0);

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(5000); 

  si_me.sin_addr.s_addr = INADDR_ANY; 
  
  int optval = 1; 

  // configure socket for reuse
  optval = 1; 
  int res = setsockopt(responseSock_, SOL_SOCKET, SO_REUSEADDR, 
	     &optval, sizeof (optval)); 
  if (res < 0) {
    throw std::runtime_error("error settng socket to reuse"); 
  }

  optval = 500000; 
  res = setsockopt (responseSock_, SOL_SOCKET, SO_RCVBUF, 
		    (const void *) &optval, sizeof(optval)); 
  if (res < 0) {
    throw std::runtime_error("error settng receive buffer size"); 

  }

  res =  bind(responseSock_, (sockaddr*)&si_me, sizeof(si_me)); 
  if (res < 0) {
    throw std::runtime_error("error binding socket"); 
  }
  
  
  ///////////////////////////////////////
  // connect response socket to epollfd, with callback
  //////////////////////////////////////
  // try adding to epoll
  ev_.events = EPOLLIN; 
  ev_.data.fd = responseSock_;
  ev_.data.ptr = this; // store self!
  res = epoll_ctl(epollFD_, EPOLL_CTL_ADD, responseSock_, &ev_); 
  
  
}

void EventSender::sendEvents(const EventTXList_t & el)
{
  boost::mutex::scoped_lock lock( appendMutex_ );
  
  // convert events to a buffer
  EventTXPending_t * etp = new EventTXPending_t; 
  etp->nonce = nonce_; 
  etp->buffer = createEventTXBuffer(nonce_, el); 
  gettimeofday(&(etp->time), NULL); 

  // append to list
  pendingRespQueue_.push_back(etp); 
  
  //send the packet
  sendPacket(etp); 

  // done
  nonce_++; 

}

void EventSender::handleReceive()
{

  boost::array<char, EBUFSIZE> recvbuffer; 
  sockaddr_in sfrom; 
  socklen_t fromlen = sizeof(sfrom); 

  int error = recvfrom(responseSock_, &recvbuffer[0], EBUFSIZE, 
		       0, (sockaddr*)&sfrom, &fromlen); 
  
  if (error > 0) 
    {
      // extract out nonce, success
      eventtxnonce_t hnonce, nnonce; 
      memcpy(&nnonce, &recvbuffer[0], sizeof(eventtxnonce_t)); 
      hnonce = ntohs(nnonce); 

      uint16_t hsuccess, nsuccess; 
      memcpy(&nsuccess, &recvbuffer[sizeof(eventtxnonce_t)], 
	     sizeof(nsuccess)); 
      hsuccess = ntohs(nsuccess); 
      
      if (hsuccess) {
	boost::mutex::scoped_lock lock( appendMutex_ );
	pendingQueue_t::iterator i; 
	for (i = pendingRespQueue_.begin(); i != pendingRespQueue_.end(); i++)
	  {
	    if ( (*i)->nonce == hnonce) {
	      pendingRespQueue_.erase(i); 
	      break;
	    }
	  }
	
      } else {
	// it was a failure? what does that even mean? 
	
	
      }
      
      
    }
  
  
}

void EventSender::checkPending()
{
  // not really sure how to do this, either
  // if it's older than some amount, resend it
  
  


}

void EventSender::sendPacket(  EventTXPending_t * etp)
{
  sendto(sendSock_, &(etp->buffer[0]), etp->buffer.size(),0, 
	 (sockaddr*)&saServer_, sizeof(saServer_)); 
  
}

EventSender::~EventSender()
{

}

#include "eventsender.h"
#include "ports.h"

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
  saServer_.sin_port = htons(EVENTTXPORT);  
  inet_aton(somaIP.c_str(), &saServer_.sin_addr); 
  
  ///////////////////////////////////////
  // connect response socket to epollfd, with callback
  //////////////////////////////////////
  // try adding to epoll
  evSock_.events = EPOLLIN; 
  //evSock_.data.fd = sendSock_; //responseSock_;
  evSock_.data.ptr = this; // store self!
  int res = epoll_ctl(epollFD_, EPOLL_CTL_ADD, sendSock_, &evSock_); 
  if (res == -1) {
    std::cout << "ERROR" << std::endl; 
  }
  ///////////////////////////////////////
  // connect pipe socket to epoll
  //////////////////////////////////////


  int pipes[2]; 
  pipe(pipes); 
  pipeR_ = pipes[0]; 
  pipeW_ = pipes[1]; 

  // try adding to epoll
  evPipe_.events = EPOLLIN; 
  //evPipe_.data.fd = pipeR_; 
  evPipe_.data.ptr = this; // store self!
  res = epoll_ctl(epollFD_, EPOLL_CTL_ADD, pipeR_, &evPipe_); 
  
  
}

void EventSender::sendEvents(const EventTXList_t & el)
{
  std::cout << "Appending events to list from master thread" << std::endl; 
  // convert events to a buffer
  EventTXPending_t * etp = new EventTXPending_t; 
  etp->nonce = nonce_; 
  etp->buffer = createEventTXBuffer(nonce_, el); 
  gettimeofday(&(etp->inserttime), NULL); 
  etp->txcnt = 0; 

  // append to list
  boost::mutex::scoped_lock lock( appendMutex_ );
  
  eventQueue_.push_back(etp); 
  
  // write to the pipe to wake up the TX handler
  char x = 0; 
  write(pipeW_, &x, 1); 
  
}

void EventSender::handleReceive(int fd)
{
  std::cout << "handleReceive fd = " << fd << ' ' << pipeR_  << std::endl; 

  if (fd == sendSock_) {
    // this is an event on the BSD socket, meaning inbound data
    newResponse(); 
  } 
  else if (fd == pipeR_) {
    // read the event
    char x; 
    read(pipeR_, &x, 1); // read from the queue 
    newEventIn(); 
  } else {
  }
  
  
}

void EventSender::newEventIn()
{
  std::cout << "newEventIn()" << std::endl; 

  // called when we are woken up from the pipe
  if ( pPendingPacket_ == 0) {
    // There's no pending packet, so we should send this new one
    boost::mutex::scoped_lock lock( appendMutex_ );
    if (! eventQueue_.empty()) {
      
      pPendingPacket_ = eventQueue_.front(); 
      eventQueue_.pop_front(); 
      
      sendPendingEvent(); 
    }
    
  }
  
}

void EventSender::newResponse()
{
  // 
  boost::array<char, EBUFSIZE> recvbuffer; 
  sockaddr_in sfrom; 
  socklen_t fromlen = sizeof(sfrom); 
  
  int error = recvfrom(sendSock_, &recvbuffer[0], EBUFSIZE, 
		       0, (sockaddr*)&sfrom, &fromlen); 
  std::cout << "newResponse()" << std::endl; 

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
      
      if (pPendingPacket_ != NULL and pPendingPacket_->nonce == hnonce) {
	
	if (hsuccess) {
	  
	  pPendingPacket_ = 0;
	  newEventIn(); 
	} else {
	  // it was a failure? what does that even mean? 
	  sendPendingEvent(); 
	  
	}
      }
      
    }
  
  
}

void EventSender::checkPending()
{
  // this can be called an arbitrary amount in the future, from ns to sec
  
  if (pPendingPacket_ == NULL) {
    // there's not a current pending packet; check queue? 
    newEventIn(); 
  } else {
    // there is a pending packet, let's use it

    timeval now; 
    gettimeofday(&now, NULL);
    
    int tsec, tusec; 
    tusec = now.tv_usec - pPendingPacket_->sendtime.tv_usec; 
    tsec = now.tv_sec - pPendingPacket_->sendtime.tv_sec; 
    int tdelta = tsec + tusec; 
    
    if (tdelta > RETXTIME ) 
      {
	if (pPendingPacket_->txcnt < RETXCNT) {
	  sendPendingEvent(); 
	} else {
	  delete pPendingPacket_;
	  pPendingPacket_ = 0; 
	  newEventIn(); 
	}
	
	
      }
  }
}
 
void EventSender::sendPendingEvent()
{
  assert (pPendingPacket_ != NULL); // sanity check
  
  sendto(sendSock_, &(pPendingPacket_->buffer[0]), 
	 pPendingPacket_->buffer.size(),0, 
	 (sockaddr*)&saServer_, sizeof(saServer_)); 
  
  pPendingPacket_->txcnt++; 
  gettimeofday(&(pPendingPacket_->sendtime), NULL); 
  lastSentNonce_ = pPendingPacket_->nonce; 

}

EventSender::~EventSender()
{
  
}

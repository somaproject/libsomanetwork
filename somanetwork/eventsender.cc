#include "eventsender.h"
#include "ports.h"

namespace somanetwork {

EventSender::EventSender(eventDispatcherPtr_t edp, pISocketProxy_t sp) :
  pDispatch_(edp),
  nonce_(0), 
  pPendingPacket_(0), 
  pSockProxy_(sp)
{

  ////////////////////////////////////////
  // configure the transmit socket
  ////////////////////////////////////////

  sendSock_ = pSockProxy_->createEventTXSocket(); 
  
  pDispatch_->addEvent(sendSock_, 
		       boost::bind(std::mem_fun(&EventSender::handleReceive),
				   this, _1)); 
  

  ///////////////////////////////////////
  // connect pipe socket to epoll
  //////////////////////////////////////


  int pipes[2]; 
  int result = pipe(pipes); 
  if (result < 0) {
    throw std::runtime_error("Error creating event sender pipe"); 
  }
  pipeR_ = pipes[0]; 
  pipeW_ = pipes[1]; 

  pDispatch_->addEvent(pipeR_, 
		       boost::bind(std::mem_fun(&EventSender::handleNewEventIn),
				   this, _1)); 
  
  
  
}

eventtxnonce_t EventSender::sendEvents(const EventTXList_t & el)
{
  // convert events to a buffer
  EventTXPending_t * etp = new EventTXPending_t; 
  eventtxnonce_t curnonce = nonce_; 
  etp->nonce = curnonce; 
  nonce_++; 
  etp->buffer = createEventTXBuffer(etp->nonce, el); 
  gettimeofday(&(etp->inserttime), NULL); 
  etp->txcnt = 0; 

  // append to list
  boost::mutex::scoped_lock lock( appendMutex_ );
  
  eventQueue_.push_back(etp); 
  
  // write to the pipe to wake up the TX handler
  char x = 0; 
  int result = write(pipeW_, &x, 1); 
  return curnonce; 
}

void EventSender::handleReceive(int fd)
{
  newResponse(); 
}

void EventSender::handleNewEventIn(int fd)
{
  char x; 
  int result = read(pipeR_, &x, 1); // read from the queue 
  newEventIn(); 
  
}

void EventSender::newEventIn()
{
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
	  delete pPendingPacket_; 
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
	  // giving up
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
	 pSockProxy_->getEventTXSockAddr(),
	 pSockProxy_->getEventTXSockAddrLen()); 

  pPendingPacket_->txcnt++; 
  gettimeofday(&(pPendingPacket_->sendtime), NULL); 
  lastSentNonce_ = pPendingPacket_->nonce; 
  
}

EventSender::~EventSender()
{
  if (  pPendingPacket_ != NULL) {
    delete pPendingPacket_; 
  }
  pSockProxy_->closeSocket(sendSock_); 
}

}

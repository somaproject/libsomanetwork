#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <boost/bind.hpp>

#include <netdb.h>
#include <stdio.h>
#include "ports.h"
#include "tests.h"
#include "eventtests.h"
#include "event.h"

FakeEventRXServer::FakeEventRXServer(skipMap_t norx, skipMap_t fail) :
  norxMap_(norx), 
  failMap_(fail),
  latestNonce_(1420)
{


}

void FakeEventRXServer::start()
{
  boost::mutex::scoped_lock lock(startmutex_); 
  
  pmainthrd_ = new boost::thread(boost::bind(&FakeEventRXServer::workthread,
					     this));
  if (not running_) {
    startcond_.wait(lock); // only return once the thread has started up
  }
  std::cout << "work thread started" << std::endl; 
}

EventTXList_t FakeEventRXServer::getRXevents()
{
  return rxevents_; 
  
}

void FakeEventRXServer::sendResponse(eventtxnonce_t nonce, bool success, 
				     sockaddr_in * to, socklen_t tolen)
{
  char buffer[4]; 
  
  eventtxnonce_t nnonce = htons(nonce); 
  memcpy(&buffer[0], &nnonce, sizeof(nnonce)); 
  
  buffer[2] = 0;
  if (success) {
    buffer[3] = 1; 
  } else {
    buffer[3] = 0; 
  }
  
  sendto(socket_, buffer, 4, 0, 
	 (sockaddr*)to, tolen); 

}

void FakeEventRXServer::socketSetup()
{


  struct sockaddr_in server;
  struct hostent *hp;

  socket_= socket(AF_INET, SOCK_DGRAM, 0);
  
  server.sin_family = AF_INET;
  hp = gethostbyname("127.0.0.1"); 
  
  bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
	hp->h_length);
  server.sin_port = htons(EVENTTXPORT);
  
  int res =  bind(socket_, (sockaddr*)&server, sizeof(server)); 
  if (res < 0) {
    std::cerr << "Error binding" << std::endl; 
  }

}
void FakeEventRXServer::workthread()
{
  socketSetup(); 
  
  // now, we're done with setup
  running_ = true; 
  std::cout << "Starting FakeEventRXServer Worker Thread" << std::endl;

  startcond_.notify_all(); 
  // receive packet
  while(running_)
    {
      sockaddr_in sfrom; 
      socklen_t fromlen = sizeof(sfrom); 
      std::vector<char> recvbuffer(2000); 
      
      
      struct timeval timeout; 
      fd_set readfds; 
      timeout.tv_sec = 1; 
      timeout.tv_usec = 0; 
      FD_ZERO(&readfds); 
      FD_SET(socket_, &readfds);
      if (select (socket_ + 1, &readfds, NULL, NULL, &timeout) < 0)
	{
	  std::cerr << "error on select" << std::endl; 
	}
      
      
      if (FD_ISSET(socket_, &readfds)) {
	
	int len = recvfrom(socket_, &recvbuffer[0], 2000, 
			   0, (sockaddr*)&sfrom, &fromlen); 
      
	std::cout << "Appending result" << std::endl; 	      
	
	if (len < 0)
	  {
	    std::cerr << "Error calling recvfrom" << std::endl; 
	  } else {
	    
	    std::cout << "recvbuffer[0]=" << int(recvbuffer[0]) 
		      << "recvbuffer[1]=" << int(recvbuffer[1]) << std::endl; 
	      
	    recvbuffer.resize(len); 
	    
	    
	    EventTXList_t elist; 
	    eventtxnonce_t nonce = getEventListFromBuffer(recvbuffer, 
							  &elist); 

	    // now the hard part
	    if (norxMap_.find(nonce) != norxMap_.end() 
		and norxMap_[nonce] > 0) {
	      // this is on the -do not respond- list. Do nothing
	      
	      norxMap_[nonce]--; 
	    } else if (failMap_.find(nonce) != failMap_.end()
		       and failMap_[nonce] > 0) {
	      // respond with failure
	      sendResponse(nonce, false, &sfrom, fromlen); 
	      failMap_[nonce]--; 
	    } else {
	      if (nonce == latestNonce_) {
		
		// respond with success but do nothing
		sendResponse(nonce, true, &sfrom, fromlen); 
	      } else {
		// respond with success
		sendResponse(nonce, true, &sfrom, fromlen); 
		
		// only update nonce when we add the events
		copy(elist.begin(), elist.end(), std::back_inserter(rxevents_)); 
		latestNonce_ = nonce; 
		startcond_.notify_all(); 
		std::cout << "received nonce " << nonce << std::endl; 
		
	      }
	      
	      
	    }
	    
	  }  
      }
    
    }
}

void FakeEventRXServer::shutdown(eventtxnonce_t waitnonce)
{
  std::cout << "attempting to shutdown, waiting for nonce = " << waitnonce << std::endl; 
  
  boost::mutex::scoped_lock lock(startmutex_); 

  while (latestNonce_ != waitnonce) {
    std::cout << "latestNonce_ = " << latestNonce_ << " so waiting for " << waitnonce << std::endl; 

    startcond_.wait(lock); 
  }

  running_ = false;   
  
  pmainthrd_->join(); 

  
}

FakeEventRXServer::~FakeEventRXServer()
{
  running_ = false;   
  
  pmainthrd_->join(); 
  
}



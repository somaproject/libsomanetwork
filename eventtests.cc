#include "tests.h"
#include "data/events.h"
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

void sendDataPacket(const std::vector<char> & dp , int port)
{
   int sock, length, n;
   struct sockaddr_in server, from;
   struct hostent *hp;
   
   sock= socket(AF_INET, SOCK_DGRAM, 0);

   server.sin_family = AF_INET;
   hp = gethostbyname("127.0.0.1"); 

   bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
         hp->h_length);
   server.sin_port = htons(port);
   length=sizeof(struct sockaddr_in);
   n=sendto(sock,&dp[0],
            sizeof(dp),0,(sockaddr*)&server,length);

}


FakeEventServer::FakeEventServer()
  running_(false)
{


}

void FakeEventServer::appendSeqsToSend(eventsetout_t es)
{
  fifo_.push(es); 

}

void FakeEventServer::start()
{
  running_ = true; 

  pretxthrd_ = new boost::thread(boost::bind(&FakeEventServer::retxthread, 
                                         this));
  pmainthrd_ = new boost::thread(boost::bind(&FakeEventServer::workthread,
                                         this));
  

  
}

void FakeEventServer::workthread()
{
  std::cout << "starting work thread" << std::endl;  
  int sock, length, n;
  struct sockaddr_in server, from;
  struct hostent *hp;

  sendsock_= socket(AF_INET, SOCK_DGRAM, 0);
  
  server.sin_family = AF_INET;
  hp = gethostbyname("127.0.0.1"); 
  
  bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
	hp->h_length);
  server.sin_port = htons(4400);
  length=sizeof(struct sockaddr_in);
  
  while (!fifo_.empty())
    {
      eventsetout_t es = fifo_.front(); 
      fifo_.pop(); 
      
      // now create the necessary event lists:
      esequence_t seq = es.first; 
      std::list<EventList_t> els; 
      for (std::vector<char>::iterator i = es.second.begin(); 
	   i != es.second.end(); i++)
	{
	  EventList_t el; 
	  // create the event
	  for (int j = 0; j < *i; i++) {
	    Event_t event; 
	    event.cmd = j; 
	    event.src = j * 4; 
	    for (int k = 0; i < 5; k++)
	      {
		event.data[k] = k * 0x1234; 
	      }
	    el.push_back(event); 
	  }
	  els.push_back(el); 
	}
      
      
      std::vector<char> dp = createEventBuffer(seq, els); 
      retxLUT_[seq] = dp; 

      n=sendto(sendsock_, &(dp)[0],
	       dp.size(), 0, (sockaddr*)&server,
	       length);
      
    }
}

void FakeEventServer::shutdown()
{
  std::cout << "Calling shutdown" << std::endl; 
  running_ = false;   
  
  pmainthrd_->join(); 
  pretxthrd_->join(); 
}

FakeEventServer::~FakeEventServer()
{
  shutdown(); 

}



void FakeEventServer::retxthread(void) {
  std::cout << "Stating reTx Thread" << std::endl; 
  // setup socket
  // 
  /////////////////////////////
  struct sockaddr_in si_me, si_other;
  int s,  slen=sizeof(si_other);
  
  s = socket(AF_INET, SOCK_DGRAM, 17); 
  if (s < 0) {
    std::cerr << "Error creating socket!" << std::endl; 
  }
  
  memset((char *) &si_me, sizeof(si_me), 0);
  
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(4400);
  si_me.sin_addr.s_addr = INADDR_ANY; 
  
  int optval = 1; 
  int res ; 
  
  optval = 1; 
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, 
	     &optval, sizeof (optval)); 
  
  res =  bind(s, (sockaddr*)&si_me, sizeof(si_me)); 
  if (res < 0) {
    std::cerr << "Error binding" << std::endl; 
  }
  
  // check for retx requests
  char dummy[100]; 


  struct sockaddr_in server;
  struct hostent *hp;

  server.sin_family = AF_INET;
  hp = gethostbyname("127.0.0.1"); 
  
  bcopy((char *)hp->h_addr, 
        (char *)&server.sin_addr,
	hp->h_length);
  server.sin_port = htons(port_);
  int length=sizeof(struct sockaddr_in);
  
  
  while(running_) {
    
    struct timeval timeout; 
    fd_set readfds; 
    timeout.tv_sec = 1; 
    timeout.tv_usec = 0; 
    FD_ZERO(&readfds); 
    FD_SET(s, &readfds);
    if (select (s + 1, &readfds, NULL, NULL, &timeout) < 0)
      {
	std::cerr << "error on select" << std::endl; 
      }
    
    if (FD_ISSET(s, &readfds)) {

      int res = recv(s, dummy, 100, 0); 
      unsigned int seq; 
      memcpy(&seq, &dummy[0], 4); 
      unsigned int seq_host; 
      seq_host = ntohl(seq); 

      std::vector<char> dp = retxLUT_[seq_host]; 

      int n = sendto(sendsock_, &(dp)[0],
		     dp.size(), 0, (sockaddr*)&server,
		     length);
      
      } else {
      }
    
  }
  close(s); 
  std::cout << "Done with retx thread" << std::endl; 
}

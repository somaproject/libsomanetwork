#include <iostream>
#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"  
#include "boost/algorithm/string/split.hpp" 
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>


#include <time.h>
#include "range.h"
#include "eventsynthtx.h"
#include "eventsource.h"

using namespace boost::filesystem; 
using namespace std; 
namespace po = boost::program_options;


uint64_t usecdelta(timeval & t1, timeval & t2) {
  uint64_t x = t1.tv_sec; 
  x = x * 1000000; 
  x += t1.tv_usec; 

  uint64_t y = t2.tv_sec; 
  y = y * 1000000; 
  y += t2.tv_usec;

  return y - x; 
  
}

int event_send_packet(eventtxparams_t *  params) {
  /*
    Handle all packet transmission, sequence updates, etc. 
    
  */
  
  pDataPacket_t p;
  size_t len; 
  // create events
  somatime_t ts = params->latesttime; 
  std::list<EventList_t> eventlists; 
  int totaleventcount = 0; 
  int tcycles = 0; 

  while ((totaleventcount < 80) and (eventlists.size() < 10) ) {
    
    Event_t event; 
    EventList_t elt; 

    // first the timer event
    event.cmd = 0x10; 
    event.src = 0; 
    event.data[0] = ts >> 32; 
    event.data[1] = ts >> 16; 
    event.data[2] = ts & 0xFFFF; 
    elt.push_back(event); 
    
    std::list<EventSource *>::iterator pes; 
    for(pes =  params->eventSources_.begin(); 
	pes != params->eventSources_.end(); ++pes) {
      (*pes)->addEvents(ts, &elt); 
    }

    // then any additional "event load" events
    for (int i =1; i < params->ecycleload; i++) {
      // load must be >= 1, i.e. the event load
      event.cmd = 0xF0; 
      event.src = 0x04; 
      event.data[0] = 0x1234; 
      event.data[1] = 0x1234; 
      event.data[2] = 0x1234; 
      event.data[3] = 0x1234; 
      event.data[4] = 0x1234; 
      elt.push_back(event); 
    }
    totaleventcount += elt.size();  
    eventlists.push_back(elt); 
    ts++; 

  }

  int CYCCNT  = eventlists.size(); 

  params->latesttime = ts; 

  // add the sequence number
  sequence_t seq = params->seq; 

  std::vector<char> buffer = createEventBuffer(seq, eventlists); 
  // now the actual TX 
  
  ssize_t result = sendto(params->socket, &buffer[0], buffer.size(), 0, 
			  (sockaddr*) &(params->si_addr), sizeof(params->si_addr)); 
  params->seq++; 

  return CYCCNT; // number of cycles
}

void event_setup_socket(eventtxparams_t *  params, std::string destipaddr) {
  /*
    open the socket 
    set the flags
  */ 
  

  short port = EVENTRXPORT; 
  
  /* Create the UDP socket */
  params->socket =  socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); 
  int optval = 1; 
  int res; 
  optval = 1; 
  setsockopt(params->socket, SOL_SOCKET, SO_REUSEADDR, 
	     &optval, sizeof (optval)); 
  
  optval = 1; 
  res = setsockopt(params->socket, SOL_SOCKET, SO_BROADCAST, 
	     &optval, sizeof (optval)); 

  /* Construct the server sockaddr_in structure */
  memset(&(params->si_addr), 0, sizeof(params->si_addr));
  
  params->si_addr.sin_family = AF_INET; 
  params->si_addr.sin_addr.s_addr  = inet_addr(destipaddr.c_str());
  params->si_addr.sin_port = htons(port); 
  
}

const double EVENTPERIOD = 20e-6; 
void eventtx_thread(eventtxparams_t*  etxp)
{
  /**
   *  txparams is the vector of transmit parameters for this thread
   *
   *
   */
  
  timeval time; 
  gettimeofday(&time, NULL); 
  sequence_t seq = 0; 
  
  // first, find the max

  etxp->elapsed = 0; 
  
  while(1) {
    timeval curtime; 
    gettimeofday(&curtime, NULL); 

    double elapsedtimeus = usecdelta(time, curtime); 
    double elapsedtime = elapsedtimeus / 1e6; 
    
    etxp->elapsed += elapsedtime; 
    int txcnt = 0;
    while(etxp->elapsed > 0) {
      int cnt = event_send_packet(etxp); 

      double chunkdur = cnt * EVENTPERIOD; 
      //std::cout << cnt << " " << chunkdur << " " <<  etxp->elapsed <<  std::endl;      
      etxp->elapsed -= chunkdur; 
      txcnt++; 

    }
    time = curtime; 
//     std::cout << "elapsed time = " << elapsedtime 
// 	      << " txcount=" << txcnt << std::endl; 
    usleep(1000); 
	  
  }
  
}


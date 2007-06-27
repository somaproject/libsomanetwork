
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "eventsender.h"
#include "eventtests.h"
#include "tests.h"

using boost::unit_test::test_suite;

const int EPOLLMAXCNT=64; 

void fakeNetworkMainLoop(int epollfd)
{
  while(1) {
    epoll_event events[EPOLLMAXCNT]; 
    const int epMaxWaitMS = 1000; 
    int nfds = epoll_wait(epollfd, events, EPOLLMAXCNT, 
			  epMaxWaitMS); 
    
    
    if (nfds > 0 ) {
      
      for(int evtnum = 0; evtnum < nfds; evtnum++) {
	PacketReceiver * pr  = (PacketReceiver*)events[evtnum].data.ptr; 
	pr->handleReceive(events[evtnum].data.fd); 
      }
      
    } else {
      // otherwise, just a timeout
    }
    
  }
  
  
}

BOOST_AUTO_TEST_CASE(simple)
{
  // set up receiver:
  skipMap_t none; 
  FakeEventRXServer fakeSoma(none, none); 
  fakeSoma.start(); 

  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 
  
  
  EventSender es(epollfd, "127.0.0.1"); 
  
  boost::thread fnml(boost::bind(&fakeNetworkMainLoop, epollfd)); 

  // generate an event list

  int nonce = 0x1234; 
  EventTX_t etx; 
  for (int i = 0; i < ADDRBITS; i++) {
    etx.destaddr[i] = 0; 
  }

  etx.destaddr[7] = true; 
  etx.destaddr[27] = true; 
  etx.destaddr[31] = true; 
  etx.destaddr[78] = true; 
  
  etx.event.src = 0x12; 
  etx.event.cmd = 0x34; 
  etx.event.data[0] = 0x0; 
  etx.event.data[1] = 0x1; 
  etx.event.data[2] = 0x2; 
  etx.event.data[3] = 0x3; 
  etx.event.data[4] = 0x4; 

  EventTXList_t etxl; 
  etxl.push_back(etx); 
  es.sendEvents(etxl); 
  
  fakeSoma.shutdown(es.getLastSentNonce()); 
  EventTXList_t recvEvents = fakeSoma.getRXevents(); 
  
  BOOST_CHECK_EQUAL(recvEvents.size(), etxl.size()); 

  
}


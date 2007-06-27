
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

BOOST_AUTO_TEST_CASE(simple)
{
  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 

  EventSender er(epollfd, "18.238.0.1"); 
  
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
  er.sendEvents(etxl); 

  
}


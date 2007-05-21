#include <boost/test/unit_test.hpp>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "eventreceiver.h"
#include "tests.h"

using boost::unit_test::test_suite;

std::list<EventPacket_t*> eventPacketBuffer; 

const int EPOLLMAXCNT=64; 

BOOST_AUTO_TEST_CASE( simpledatatest )
{
  // Can we send a single packet? this is the model for all future activity
  // 

  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 
 
}


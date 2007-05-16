#include <boost/test/unit_test.hpp>

#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "datareceiver.h"
#include "tests.h"

using boost::unit_test::test_suite;

std::list<RawData*> rawDataBuffer; 
void append(RawData * rdp)
{
  rawDataBuffer.push_back(rdp); 
}

const int EPOLLMAXCNT=64; 


BOOST_AUTO_TEST_CASE( simpledatatest )
{
  // Can we send a single packet? this is the model for all future activity
  // 

  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 
  int src = 10;
  int typ = 2; 
  
  DataReceiver dr(epollfd, src, typ, &append); 
  
  // validate epoll addition
  
  // now we send packets to the local 
  struct sockaddr_in servaddr; 
  servaddr.sin_family= AF_INET; 
  servaddr.sin_port = htons(dataPortLookup(typ, src)); 
  inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); 
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0 ); 
  char * fdp = fakeDataPacket(0, src, typ); 

  delete fdp; 


  sockfd
}


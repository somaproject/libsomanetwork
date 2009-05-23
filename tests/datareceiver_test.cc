#include <boost/test/unit_test.hpp>

#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <somanetwork/datareceiver.h>
#include <somanetwork/eventdispatcher.h>
#include <somanetwork/sockproxy.h>
#include <somanetwork/netsockproxy.h>

#include "tests.h"

using boost::unit_test::test_suite;
using namespace somanetwork; 



BOOST_AUTO_TEST_SUITE(datareceivertest)


std::list<pDataPacket_t> rawDataBuffer; 

void clearBuffer() {
  rawDataBuffer.clear(); 
}

void append(pDataPacket_t rdp)
{
  rawDataBuffer.push_back(rdp); 
}

BOOST_AUTO_TEST_CASE( simpledatatest )
{
  //
  // Can we send a single packet? this is the model for all future activity
  // 
  clearBuffer(); 
  
  // and then test them all. 
  eventDispatcherPtr_t ped(new EventDispatcher()); 
  datasource_t src = 10;
  datatype_t typ = RAW; 
  pISocketProxy_t sp(new NetSocketProxy("127.0.0.1")); 

  DataReceiver dr(ped, sp, src, typ, &append); 
  
  // validate epoll addition
  FakeDataServer server(typ, src); 
  const int SEQ = 1000; 
  server.appendSeqsToSend(SEQ); 
  
  
  server.start(); 
  ped->runonce(); 
  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 1); 
  BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, SEQ); 
  
  clearBuffer(); 
  
}

BOOST_AUTO_TEST_CASE(outofordertest)
{
  // we will send a series of packets out order; 
  
  rawDataBuffer.clear(); 
  
  // and then test them all. 
  eventDispatcherPtr_t ped(new EventDispatcher()); 
  
  datasource_t src = 30;
  datatype_t  typ = TSPIKE; 

  pISocketProxy_t sp(new NetSocketProxy("127.0.0.1")); 

  DataReceiver dr(ped, sp, src, typ, &append); 
  
  // validate epoll addition
  FakeDataServer server(typ, src); 
  
  std::vector<sequence_t> seqs; 
  seqs.push_back(0); 
  seqs.push_back(1); 
  seqs.push_back(2); 
  seqs.push_back(4); 
  seqs.push_back(5); 
  seqs.push_back(3); 
  seqs.push_back(6); 
  seqs.push_back(7); 
  

  server.appendSeqsToSend(seqs); 

  server.start(); 
  for (int i = 0; i < 8; i++) {
    ped->runonce(); 
  }  

  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 8); 
  for(int i = 0; i < 8; i++)
    {
      BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, i); 
      rawDataBuffer.pop_front(); 
    }
  clearBuffer(); 

}

BOOST_AUTO_TEST_CASE(dupetest)
{
  // we will send a series of packets with a dupe
  clearBuffer(); 

  // and then test them all. 
  eventDispatcherPtr_t ped(new EventDispatcher()); 

  datasource_t src = 30;
  datatype_t typ = TSPIKE; 

  pISocketProxy_t sp(new NetSocketProxy("127.0.0.1")); 
  
  DataReceiver dr(ped, sp, src, typ, &append); 
  
  // validate epoll addition
  FakeDataServer server(typ, src); 

  std::vector<sequence_t> seqs; 
  seqs.push_back(0); 
  seqs.push_back(1); 
  seqs.push_back(2); 
  seqs.push_back(3); 
  seqs.push_back(4); 
  seqs.push_back(4); 
  seqs.push_back(5); 
  seqs.push_back(6); 
  seqs.push_back(7); 
  

  server.appendSeqsToSend(seqs); 

  struct epoll_event ev;
  epoll_event events[EPOLLMAXCNT];
  
  server.start(); 
  for (int i = 0; i < 9; i++) {
    ped->runonce(); 
  }  

  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 8); 
  for(int i = 0; i < 8; i++)
    {
      BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, i); 
      rawDataBuffer.pop_front(); 
      
    }
  clearBuffer(); 

}

BOOST_AUTO_TEST_CASE(retxtest)
{
  // omit sending completely, and verify that we get thee relevant retx req
  
  rawDataBuffer.clear(); 
  
  // and then test them all. 
  eventDispatcherPtr_t ped(new EventDispatcher()); 
  datasource_t src = 30;
  datatype_t typ = RAW; 
  int epollfd = epoll_create(EPOLLMAXCNT);

  pISocketProxy_t sp(new NetSocketProxy("127.0.0.1")); 

  DataReceiver dr(ped, sp, src, typ, &append); 
  
  // validate epoll addition
  FakeDataServer server(typ, src); 
  
  std::vector<sequence_t> seqs; 
  seqs.push_back(0); 
  seqs.push_back(1); 
  seqs.push_back(2); 
  seqs.push_back(3); 
  seqs.push_back(4); 
  seqs.push_back(6); 
  seqs.push_back(7); 
  seqs.push_back(8); 
  seqs.push_back(9); 
  

  server.appendSeqsToSend(seqs); 

  struct epoll_event ev;
  epoll_event events[EPOLLMAXCNT];
  
  server.start(); 
  for (int i = 0; i < 10; i++) {
    ped->runonce(); 
  }  
  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 10); 
  for(int i = 0; i < rawDataBuffer.size(); i++)
    {
      BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, i); 
      
      rawDataBuffer.pop_front(); 
      
    }
  clearBuffer();
}

BOOST_AUTO_TEST_SUITE_END()

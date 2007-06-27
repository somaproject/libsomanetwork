#include <boost/test/unit_test.hpp>

#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "datareceiver.h"
#include "tests.h"

using boost::unit_test::test_suite;

std::list<DataPacket_t*> rawDataBuffer; 
void clearBuffer() {
   std::list<DataPacket_t*>::iterator i ; 
   for (i = rawDataBuffer.begin(); i != rawDataBuffer.end(); i++)
     {
       delete *i; 
       
     }
   rawDataBuffer.clear(); 
}

void append(DataPacket_t * rdp)
{
  rawDataBuffer.push_back(rdp); 
}

const int EPOLLMAXCNT=64; 


BOOST_AUTO_TEST_CASE( simpledatatest )
{
  // Can we send a single packet? this is the model for all future activity
  // 
  clearBuffer(); 

  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 
  datasource_t src = 10;
  datatype_t typ = RAW; 
  
  DataReceiver dr(epollfd, src, typ, &append); 
  
  // validate epoll addition
  FakeDataServer server(typ, src); 
  const int SEQ = 1000; 
  server.appendSeqsToSend(SEQ); 

  struct epoll_event ev;
  epoll_event events[EPOLLMAXCNT];
  
  server.start(); 
  int nfds = epoll_wait(epollfd, events, EPOLLMAXCNT, -1);
  
  for(int evtnum = 0; evtnum < nfds; evtnum++) {
    DataReceiver * drp  = (DataReceiver*)events[evtnum].data.ptr; 
    drp->handleReceive(); 
  }
  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 1); 
  BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, SEQ); 
  clearBuffer(); 

}

BOOST_AUTO_TEST_CASE(outofordertest)
{
  // we will send a series of packets out order; 
  
  rawDataBuffer.clear(); 

  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 
  datasource_t src = 30;
  datatype_t  typ = TSPIKE; 
  
  DataReceiver dr(epollfd, src, typ, &append); 
  
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

  struct epoll_event ev;
  epoll_event events[EPOLLMAXCNT];
  
  server.start(); 
  for (int i = 0; i < 8; i++) {
    int nfds = epoll_wait(epollfd, events, EPOLLMAXCNT, -1);
  
    for(int evtnum = 0; evtnum < nfds; evtnum++) {
      DataReceiver * drp  = (DataReceiver*)events[evtnum].data.ptr; 
      drp->handleReceive(); 
    }
  }  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 8); 
  for(int i = 0; i < 8; i++)
    {
      BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, i); 
      delete rawDataBuffer.front(); 
      rawDataBuffer.pop_front(); 
    }
  clearBuffer(); 

}

BOOST_AUTO_TEST_CASE(dupetest)
{
  // we will send a series of packets with a dupe
  clearBuffer(); 

  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 
  datasource_t src = 30;
  datatype_t typ = TSPIKE; 
  
  DataReceiver dr(epollfd, src, typ, &append); 
  
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
    int nfds = epoll_wait(epollfd, events, EPOLLMAXCNT, -1);

    for(int evtnum = 0; evtnum < nfds; evtnum++) {
      DataReceiver * drp  = (DataReceiver*)events[evtnum].data.ptr; 
      drp->handleReceive(); 
    }
  }  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 8); 
  for(int i = 0; i < 8; i++)
    {
      BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, i); 
      delete rawDataBuffer.front(); 
      rawDataBuffer.pop_front(); 
      
    }
  clearBuffer(); 

}

BOOST_AUTO_TEST_CASE(retxtest)
{
  // omit sending completely, and verify that we get thee relevant retx req
  
  rawDataBuffer.clear(); 

  // and then test them all. 
  int epollfd = epoll_create(EPOLLMAXCNT); 
  datasource_t src = 30;
  datatype_t typ = RAW; 
  
  DataReceiver dr(epollfd, src, typ, &append); 
  
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
    int nfds = epoll_wait(epollfd, events, EPOLLMAXCNT, -1);

    for(int evtnum = 0; evtnum < nfds; evtnum++) {
      DataReceiver * drp  = (DataReceiver*)events[evtnum].data.ptr; 
      drp->handleReceive(); 
    }
  }  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 10); 
  for(int i = 0; i < rawDataBuffer.size(); i++)
    {
      BOOST_CHECK_EQUAL(rawDataBuffer.front()->seq, i); 
      delete rawDataBuffer.front(); 
     
      rawDataBuffer.pop_front(); 
      
    }
  clearBuffer();
}


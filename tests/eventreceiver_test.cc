#include <boost/test/unit_test.hpp>

#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "eventreceiver.h"
#include "eventtests.h"
#include "tests.h"

BOOST_AUTO_TEST_SUITE(datareceiver_test)

using boost::unit_test::test_suite;

std::list<EventList_t*> eventListBuffer; 

void append(EventList_t * elp)
{
  eventListBuffer.push_back(elp); 
}

void verifyEventListBuffer(std::vector<std::vector<char> > & inputbufs)
{

  std::list<EventList_t *>::iterator elbuf = eventListBuffer.begin(); 
  
  for (std::vector<std::vector<char> >::iterator esch = inputbufs.begin(); 
       esch != inputbufs.end(); esch++)
    {
      std::list<EventList_t> el = genEventList(*esch);  
      EventList_t::iterator curevt = (*elbuf)->begin(); 
      int ecnt = 0; 
      for (std::list<EventList_t>::iterator i = el.begin(); 
	   i != el.end(); i++)
	{
	  for (EventList_t::iterator j = i->begin();
	       j != i->end(); j++)
	    {
	      BOOST_CHECK_EQUAL(j->cmd, curevt->cmd); 
	      BOOST_CHECK_EQUAL(j->src, curevt->src); 
	      for (int z = 0; z < 5; z++)
		{
		  BOOST_CHECK_EQUAL(j->data[z], curevt->data[z]); 
		}
	      curevt++; 
	    }
	}
      elbuf++; 
    }
  
}

BOOST_AUTO_TEST_CASE( simpleeventtest )
{
  // 
  // Can we send a single packet? this is the model for all later tests
  // 
  eventListBuffer.clear(); 

  // and then test them all. 
  eventDispatcherPtr_t ped(new EventDispatcher()); 
  EventReceiver er(ped, &append); 
  
  // validate epoll addition
  FakeEventServer server; 
  // create fake data
  eventsetout_t eso; 
  eso.first = 0x12345678; 
  std::vector<char> lens; 
  lens.push_back(4); 
  lens.push_back(8); 
  lens.push_back(12); 
  lens.push_back(20); 
  lens.push_back(30); 
  
  eso.second =   lens; 
  server.appendSeqsToSend(eso); 

  server.start(); 
  ped->runonce(); 
  
  BOOST_CHECK_EQUAL(eventListBuffer.size(), 1); 
  BOOST_CHECK_EQUAL(eventListBuffer.front()->size(), 4+8+12+20+30); 

  


  // assert data values
  std::vector<std::vector<char> > outlist; 
  outlist.push_back(lens); 
  verifyEventListBuffer(outlist); 

}


BOOST_AUTO_TEST_CASE( multieventtest )
{
  // Can we get multiple packets
  
  eventListBuffer.clear(); 

  // and then test them all. 

  eventDispatcherPtr_t ped(new EventDispatcher()); 
  EventReceiver er(ped, &append); 
  
  // validate epoll addition
  FakeEventServer server; 
  // create fake data

  std::vector<std::vector<char> > outlist; 

  eventsetout_t eso; 
  eventseq_t seq = 0x12340000; 
  for (int i = 0; i < 10; i++) {
    std::vector<char> lens; 
    lens.push_back(4+i*2); 
    lens.push_back(8+i); 
    lens.push_back(12+i); 
    if (i % 2 == 0) {
      lens.push_back(20+i); 
      lens.push_back(17+i); 
    }
    outlist.push_back(lens); 

    eso.first = seq;   
    seq++; 
    eso.second = lens; 

    server.appendSeqsToSend(eso); 
  }

  server.start(); 

  /// wait for the primary rx to complete
  while ( !server.workthreaddone()) {
    ped->runonce(); 
  }
  
  // assert data values
  BOOST_CHECK_EQUAL(eventListBuffer.size(), 10); 

  verifyEventListBuffer(outlist); 

}

BOOST_AUTO_TEST_CASE( outofordertest )
{
  //
  // Check for proper handling of soma-device tx of out-of-order
  // packets
  // 

  eventListBuffer.clear(); 

  // and then test them all. 
  eventDispatcherPtr_t ped(new EventDispatcher()); 
  EventReceiver er(ped, &append); 
  
  // validate epoll addition
  FakeEventServer server; 
  // create fake data

  std::vector<std::vector<char> > outlist; 

  eventsetout_t eso; 
  eventseq_t seqs[] = {0, 1, 2, 3, 4, 5, 7, 6, 8, 9}; 
  
  for (int i = 0; i < 10; i++) {
    std::vector<char> lens; 
    lens.push_back(1+i*2); 
    lens.push_back(4+i); 
    lens.push_back(3+i); 

    if (i % 2 == 0) {
      lens.push_back(2+i); 
      lens.push_back(7+i); 
    }
    outlist.push_back(lens); 
  }

  for (int i = 0; i < 10; i++) {
    
    eso.first = seqs[i];   
    eso.second = outlist[seqs[i]]; 

    server.appendSeqsToSend(eso); 
  }

  server.start(); 

  while ( !server.workthreaddone()) {
    ped->runonce(); 
  }
      
  // assert data values
  BOOST_CHECK_EQUAL(eventListBuffer.size(), 10); 

  verifyEventListBuffer(outlist); 

}

BOOST_AUTO_TEST_CASE( droptest )
{
  // if we send packets out of order, do we deal 
  
  eventListBuffer.clear(); 
  
  // and then test them all. 
  eventDispatcherPtr_t ped(new EventDispatcher()); 
  EventReceiver er(ped, &append); 
  
  // validate epoll addition
  FakeEventServer server; 
  // create fake data
  server.setSkip(4);  // set skip value
  server.setSkip(7);  // set skip value

  std::vector<std::vector<char> > outlist; 

  eventsetout_t eso; 
  eventseq_t seqs[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; 
  
  for (int i = 0; i < 10; i++) {
    std::vector<char> lens; 
    lens.push_back(1+i*2); 
    lens.push_back(4+i); 
    lens.push_back(3+i); 

    if (i % 2 == 0) {
      lens.push_back(2+i); 
      lens.push_back(7+i); 
    }
    outlist.push_back(lens); 
  }

  for (int i = 0; i < 10; i++) {
    
    eso.first = seqs[i];   
    eso.second = outlist[seqs[i]]; 

    server.appendSeqsToSend(eso); 
  }


  server.start(); 


  while ( !server.workthreaddone()) {
    ped->runonce(); 
  }

  
  // assert data values
  BOOST_CHECK_EQUAL(eventListBuffer.size(), 10); 
  
  verifyEventListBuffer(outlist); 

}


BOOST_AUTO_TEST_SUITE_END()

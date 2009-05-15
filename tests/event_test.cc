#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>             
#include <assert.h>       
#include <somanetwork/soma_event.h>
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(event_test); 

void vectbufcopy(boost::array<char, EBUFSIZE>* outbuf, 
		std::vector<char> & inbuf)
{
  if (inbuf.size() >= EBUFSIZE) {
    std::cout << "Warning, inbuf too large" << std::endl;
  }
  memcpy(&((*outbuf)[0]), &inbuf[0], inbuf.size()); 
}

BOOST_AUTO_TEST_CASE(event_empty)
{

  std::vector<char> buffer(4+5*2);
  buffer[0] = 0x01; 
  buffer[1] = 0x23; 
  buffer[2] = 0x45; 
  buffer[3] = 0x67; 
  for (int i = 4; i < (4+5*2); i++)
    {
      buffer[i] = 0; 
    }
  
  boost::array<char, EBUFSIZE> inbuffer; 
  vectbufcopy(&inbuffer, buffer); 
  EventPacket_t * test = newEventPacket(inbuffer, buffer.size()); 
  BOOST_CHECK_EQUAL(test->seq , 0x01234567); 
  BOOST_CHECK_EQUAL(test->events->size(), 0); 
  
}

BOOST_AUTO_TEST_CASE(event_sets_frompy)
{
  std::fstream pyfile("events.1.dat", ios::in | ios::binary); 

  std::vector<char> buffer;
  
  while (! pyfile.eof() ) 
    {
      char x; 
      pyfile.read(&x, 1); 
      buffer.push_back(x); 
    }

  boost::array<char, EBUFSIZE> inbuffer; 
  vectbufcopy(&inbuffer, buffer); 
  

  EventPacket_t * test = newEventPacket(inbuffer, buffer.size()); 
  
  BOOST_CHECK_EQUAL(test->seq , 0x12345678); 
  BOOST_CHECK_EQUAL(test->events->size(), 89); 
  
  int lens[] = {5, 17, 41, 23, 3}; 
  int epos = 0; 
  EventList_t * el = test->events; 

  for (int esetnum = 0; esetnum < 5; esetnum++)
    {
      for (int i = 0; i < lens[esetnum]; i++)
	{
	  BOOST_CHECK_EQUAL((*el)[epos].cmd, i); 
	  BOOST_CHECK_EQUAL((*el)[epos].src, (i*7) % 256); 
	  for (int k = 0; k < 5; k++)
	    {
	      uint16_t data; 
	      data = ntohs((*el)[epos].data[k]); 
	      BOOST_CHECK_EQUAL(data , i + k * 718 +314) ; 
	    }
	  epos++; 
	}
    }

  
}

BOOST_AUTO_TEST_SUITE_END(); 

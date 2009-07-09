#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>             
#include <assert.h>       
#include <somanetwork/event.h>
#include <boost/lexical_cast.hpp>

using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(event_test); 

using namespace somanetwork; 

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
  pEventPacket_t test = newEventPacket(inbuffer, buffer.size()); 
  BOOST_CHECK_EQUAL(test->seq , 0x01234567); 
  BOOST_CHECK_EQUAL(test->events->size(), 0); 
  
}

BOOST_AUTO_TEST_CASE(event_cout)
{
  Event_t e; 
  e.cmd = 16; 
  e.src = 128;
  e.data[0] = 0x1234;
  e.data[1] = 0x1122;
  e.data[2] = 0x3344;
  e.data[3] = 0x5566;
  e.data[4] = 0x7788;
  
  std::stringstream interpreter;

  interpreter << e << std::endl; 
  std::string evt = boost::lexical_cast<std::string>(e); 
  std::string expected("event: cmd=10 src=80 1234 1122 3344 5566 7788"); 
  BOOST_CHECK_EQUAL(evt, expected); 
}



// BOOST_AUTO_TEST_CASE(event_sets_frompy)
// {
//   std::fstream pyfile("events.1.dat", ios::in | ios::binary); 

//   std::vector<char> buffer;
  
//   while (! pyfile.eof() ) 
//     {
//       char x; 
//       pyfile.read(&x, 1); 
//       buffer.push_back(x); 
//     }

//   boost::array<char, EBUFSIZE> inbuffer; 
//   vectbufcopy(&inbuffer, buffer); 
  

//   EventPacket_t * test = newEventPacket(inbuffer, buffer.size()); 
  
//   BOOST_CHECK_EQUAL(test->seq , 0x12345678); 
//   BOOST_CHECK_EQUAL(test->events->size(), 89); 
  
//   int lens[] = {5, 17, 41, 23, 3}; 
//   int epos = 0; 
//   EventList_t * el = test->events; 

//   for (int esetnum = 0; esetnum < 5; esetnum++)
//     {
//       for (int i = 0; i < lens[esetnum]; i++)
// 	{
// 	  BOOST_CHECK_EQUAL((*el)[epos].cmd, i); 
// 	  BOOST_CHECK_EQUAL((*el)[epos].src, (i*7) % 256); 
// 	  for (int k = 0; k < 5; k++)
// 	    {
// 	      uint16_t data; 
// 	      data = ntohs((*el)[epos].data[k]); 
// 	      BOOST_CHECK_EQUAL(data , i + k * 718 +314) ; 
// 	    }
// 	  epos++; 
// 	}
//     }

  
// }

BOOST_AUTO_TEST_SUITE_END(); 

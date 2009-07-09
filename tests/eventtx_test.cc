#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>             
#include <assert.h>       
#include <somanetwork/eventtx.h>
#include <boost/lexical_cast.hpp>

using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(eventtx_test); 

using namespace somanetwork; 
BOOST_AUTO_TEST_CASE(eventtx_empty)
{
  
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

  std::vector<char> buffer = createEventTXBuffer(nonce, etxl); 

  //now try and recover it
  EventTXList_t retxl; 
  eventtxnonce_t rnonce = getEventListFromBuffer(buffer, &retxl); 
  
  BOOST_CHECK_EQUAL(nonce, rnonce); 
  
  BOOST_CHECK_EQUAL(1, retxl.size()); 
  
  // verify address
  EventTX_t retx = retxl.front(); 
  for (int i = 0; i < ADDRBITS; i++) {
    BOOST_CHECK_EQUAL(etx.destaddr[0], retx.destaddr[0]); 
  }
  
  
}


BOOST_AUTO_TEST_CASE(eventtx_cout)
{
  EventTX_t e; 
  e.destaddr[2] = true; 
  e.destaddr[32] = true; 
  
  e.event.cmd = 16; 
  e.event.src = 128;
  e.event.data[0] = 0x1234;
  e.event.data[1] = 0x1122;
  e.event.data[2] = 0x3344;
  e.event.data[3] = 0x5566;
  e.event.data[4] = 0x7788;
  
  std::string evt = boost::lexical_cast<std::string>(e); 
  std::string expected("eventtx: dest: 02 20 (event: cmd=10 src=80 1234 1122 3344 5566 7788)"); 
  BOOST_CHECK_EQUAL(evt, expected); 

}

BOOST_AUTO_TEST_CASE(eventtxbcast_cout)
{
  EventTX_t e; 
  for(int i = 0; i < ADDRBITS; i++) {
    e.destaddr[i] = true; 
  }

  e.event.cmd = 16; 
  e.event.src = 128;
  e.event.data[0] = 0x1234;
  e.event.data[1] = 0x1122;
  e.event.data[2] = 0x3344;
  e.event.data[3] = 0x5566;
  e.event.data[4] = 0x7788;
  
  std::string evt = boost::lexical_cast<std::string>(e); 
  std::string expected("eventtx: dest: bcast all (event: cmd=10 src=80 1234 1122 3344 5566 7788)"); 
  BOOST_CHECK_EQUAL(evt, expected); 

}




BOOST_AUTO_TEST_SUITE_END(); 

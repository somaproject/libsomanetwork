#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>             
#include <assert.h>       
#include "eventtx.h"
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(eventtx_test); 


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


BOOST_AUTO_TEST_SUITE_END(); 


#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>    
#include <fstream>                    
#include <somanetwork/raw.h>
#include "canonical.h"

using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

using namespace somanetwork;

BOOST_AUTO_TEST_SUITE(raw_test); 

BOOST_AUTO_TEST_CASE(raw_toraw)
{
  /*
    Generate Raw packet, convert to datapacket, and then read it back in. 
    
   */ 
  for( int src = 0; src < 64; src++) {
    for (int index = 0; index < 1000; index++) {
      Raw_t raw = generateCanonicalRaw(src, index); 
      pDataPacket_t dpt = rawFromRaw(raw); 
  
      Raw_t wr = rawToRaw(dpt); 
  
      test_equality(raw, wr); 
    }
  }

}


BOOST_AUTO_TEST_SUITE_END(); 

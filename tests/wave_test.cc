
#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>    
#include <fstream>                    
#include "somanetwork/wave.h"
#include "canonical.h"

using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

using namespace somanetwork; 

BOOST_AUTO_TEST_SUITE(wave_test); 

BOOST_AUTO_TEST_CASE(wave_toraw)
{
  /* 
     Generate a Wave packet, convert to a datapacket, and read it back in
   */
  for( int src = 0; src < 64; src++) {
    for (int index = 0; index < 1000; index++) {
      Wave_t wave = generateCanonicalWave(src, index); 
      pDataPacket_t dpt = rawFromWave(wave); 
  
      Wave_t wr = rawToWave(dpt); 
  
      test_equality(wave, wr); 
    }
  }
  
}


BOOST_AUTO_TEST_SUITE_END(); 

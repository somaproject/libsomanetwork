
#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>    
#include <fstream>                    
#include <somanetwork/raw.h>
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

using namespace somanetwork;

BOOST_AUTO_TEST_SUITE(raw_test); 

BOOST_AUTO_TEST_CASE(raw_toraw)
{
  Raw_t raw; 
  raw.src = 120; 
  raw.time = 0x123456789ABCDEF; 
  raw.chansrc = 0x4567; 
  raw.filterid = 0x12342233; 
  // fil the raw with 128 words
  for (int i = 0; i < RAWBUF_LEN; i++)
    {
      raw.data[i] = (i * 1000); 
    }
  
  pDataPacket_t dpt = rawFromRaw(raw); 
  
  Raw_t wr = rawToRaw(dpt); 
  
  BOOST_CHECK_EQUAL(wr.src, raw.src); 
  BOOST_CHECK_EQUAL(wr.time, raw.time); 

  BOOST_CHECK_EQUAL(wr.chansrc, raw.chansrc); 

  BOOST_CHECK_EQUAL(wr.filterid, raw.filterid); 

  for (int i = 0; i < RAWBUF_LEN; i++)
    {
      BOOST_CHECK_EQUAL(wr.data[i], i*1000); 
    }

}


BOOST_AUTO_TEST_SUITE_END(); 

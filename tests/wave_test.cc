
#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>    
#include <fstream>                    
#include "somanetwork/wave.h"
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

using namespace somanetwork; 

BOOST_AUTO_TEST_SUITE(wave_test); 

BOOST_AUTO_TEST_CASE(wave_toraw)
{
  Wave_t w; 
  w.src = 120; 
  w.time = 0x123456789ABCDEF; 
  w.sampratenum = 0xCD12; 
  w.selchan = 0x4567; 
  w.filterid = 0x1234; 
  // fil the wave with 128 words
  for (int i = 0; i < WAVEBUF_LEN; i++)
    {
      w.wave[i] = (i * 1000); 
    }
  
  pDataPacket_t dpt = rawFromWave(w); 
  
  Wave_t wr = rawToWave(dpt); 
  
  BOOST_CHECK_EQUAL(wr.src, w.src); 
  BOOST_CHECK_EQUAL(wr.time, w.time); 

  BOOST_CHECK_EQUAL(wr.sampratenum, w.sampratenum); 
  
  BOOST_CHECK_EQUAL(wr.selchan, w.selchan); 

  BOOST_CHECK_EQUAL(wr.filterid, w.filterid); 

  for (int i = 0; i < 128; i++)
    {
      BOOST_CHECK_EQUAL(wr.wave[i], i*1000); 
    }

}


BOOST_AUTO_TEST_SUITE_END(); 

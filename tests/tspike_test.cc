
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>                    
#include <somanetwork/tspike.h>
#include "canonical.h"
#include "test_config.h"

using namespace boost;       
using namespace boost::filesystem; 
using namespace std; 
using namespace somanetwork; 

BOOST_AUTO_TEST_SUITE(tspike_test); 

BOOST_AUTO_TEST_CASE(tspike_toandfrom) 
{
  /*
    Identity operation -- convert a TSpike_t to a data packet
    and then convert back, and make sure we are getting
    the same data. 

   */ 
  for (int i = 0; i < 20; i++) { 
    for (int j = 0; j < 10; j++) { 
      TSpike_t ts = generateCanonicalTSpike(i, j) ; 
      
      pDataPacket_t rdp = rawFromTSpike(ts); 

      TSpike_t tsr = rawToTSpike(rdp); 
      test_equality(ts, tsr); 
    }
  }
  
}


BOOST_AUTO_TEST_CASE(tspike_fromrawpy)
{
  /*
    We generate test data in python with tspike_test.py and read it here
    
  */

  path datadir(TEST_DATA_PATH); 
  path datafile = datadir / path("tspikes.frompy.dat"); 
  if (!exists(datafile)) {
    throw std::runtime_error("Test could not find tspikes.frompy.dat"); 
  }
  
  std::fstream pyfile(datafile.string().c_str(), ios::in | ios::binary); 
  int N = 10000; 
  for (int i = 0; i < N; i++)
    {
      pDataPacket_t rdp(new DataPacket_t()); 
      const int PACKLEN = 574; 
      char buffer[PACKLEN]; 

      pyfile.read(buffer, PACKLEN); 
      memcpy(&rdp->body[0], buffer, PACKLEN); 

      TSpike_t ts = rawToTSpike(rdp); 
      BOOST_CHECK_EQUAL(ts.src,  i % 256); 
      uint64_t time = i * 10215 + 0x12345678ll; 
      std::cout << "ts.time = " <<  ts.time << std::endl; 
      BOOST_CHECK_EQUAL(ts.time, time); 
      
      std::vector<TSpikeWave_t> waves; 
      waves.push_back(ts.x); 
      waves.push_back(ts.y); 
      waves.push_back(ts.a); 
      waves.push_back(ts.b); 

      for (int j = 0; j < 4; j++)
	{
	  TSpikeWave_t tsw = waves[j] ; 
	  BOOST_CHECK_EQUAL(tsw.filtid, (j*i)  );
	  BOOST_CHECK_EQUAL(tsw.valid, (j*i) % 256 );
	  BOOST_CHECK_EQUAL(tsw.threshold, j * i * (2*17 - 141)); 
	  for (int k = 0; k < TSPIKEWAVE_LEN; k++)
	    {
	      uint64_t val = (j*i*0x12345 + k); 
	      val = val % (1<<31); 
	      BOOST_CHECK_EQUAL(tsw.wave[k], val); 
	    }
	}
    }
  
    

}


BOOST_AUTO_TEST_SUITE_END(); 

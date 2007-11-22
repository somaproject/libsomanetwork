
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>                    
#include "tspike.h"
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(tspike_test); 

BOOST_AUTO_TEST_CASE(tspike_fromrawpy)
{
  // we generate test data in python and read it here
  std::fstream pyfile("tspikes.frompy.dat", ios::in | ios::binary); 
  int N = 10000; 
  for (int i = 0; i < N; i++)
    {
      pDataPacket_t rdp(new DataPacket_t()); 
      const int PACKLEN = 548; 
      char buffer[PACKLEN]; 

      pyfile.read(buffer, PACKLEN); 
      memcpy(&rdp->body[0], buffer, PACKLEN); 

      TSpike_t ts = rawToTSpike(rdp); 
      BOOST_CHECK_EQUAL(ts.src,  i % 256); 
      uint64_t time = i * 10215; 

      BOOST_CHECK_EQUAL(ts.time, time); 
      
      std::vector<TSpikeWave_t> waves; 
      waves.push_back(ts.x); 
      waves.push_back(ts.y); 
      waves.push_back(ts.a); 
      waves.push_back(ts.b); 

      for (int j = 0; j < 4; j++)
	{
	  TSpikeWave_t tsw = waves[j] ; 
	  BOOST_CHECK_EQUAL(tsw.filtid, (j*i) % 256 );
	  BOOST_CHECK_EQUAL(tsw.valid, (j*i) % 256 );
	  BOOST_CHECK_EQUAL(tsw.threshold, j * i * (2*17 - 141)); 
	  for (int k = 0; k < TSPIKEWAVE_LEN; k++)
	    {
	      BOOST_CHECK_EQUAL(tsw.wave[k], j*i*0x12345 + k); 
	    }
	}
    }
  
    

}

// BOOST_AUTO_TEST_CASE(tspike_toraw)
// {
//   // we generate test data in python and read it here
//   std::fstream pyfile("frompy.dat", ios::in | ios::binary); 
//   int N = 1; 
//   for (int i = 0; i < N; i++)
//     {
//       pDataPacket_t rdp(new DataPacket_t()); 
//       const int PACKLEN = 548; 
//       char buffer[PACKLEN]; 

//       pyfile.read(buffer, PACKLEN); 
//       memcpy(&rdp->body[0], buffer, PACKLEN); 

//       TSpike_t ts = rawToTSpike(rdp); 
      
      
//       pDataPacket_t nrdp = rawFromTSpike(ts); 
      
//       for (int i = 0; i < PACKLEN; i++)
// 	{
// // 	  std::cout << i << ':' <<  hex << (uint32_t)(rdp->body[i]) << ' ' 
// // 		    << (uint16_t)( nrdp->body[i]) << std::endl; 
// 	  BOOST_CHECK_EQUAL(rdp->body[i], nrdp->body[i]); 
// 	}
//     }
  
  

// }


BOOST_AUTO_TEST_SUITE_END(); 

#include "canonical.h"

namespace somanetwork { 

  Raw_t generateCanonicalRaw(datasource_t src, int index)
  {
    Raw_t r; 
    r.src = src; 
    r.time = index + 0xAABBCCDD; 
    r.chansrc = index * 3; 
    r.filterid = 0x1122 * (index + 10); 
    for(int i = 0; i < RAWBUF_LEN; i++) {
      r.data[i] = i * (index + 13);     
    }
    return r;
  }
  
  void test_equality(const Raw_t & r1, const Raw_t & r2) 
  {
    BOOST_CHECK_EQUAL(r1.src, r2.src); 
    BOOST_CHECK_EQUAL(r1.time, r2.time); 
    BOOST_CHECK_EQUAL(r1.chansrc, r2.chansrc); 
    BOOST_CHECK_EQUAL(r1.filterid, r2.filterid); 
    for(int i = 0; i < RAWBUF_LEN; i++) {
      BOOST_CHECK_EQUAL(r1.data[i], r2.data[i]); 
    }
  }
  
  
  TSpike_t generateCanonicalTSpike(datasource_t src, int index) 
  {
    TSpike_t ts; 
    ts.src = src; 
    ts.time = 0x1234 * (index + 7); 
    
    TSpikeWave_t * wave[] = {&ts.x, &ts.y, &ts.a, &ts.b}; 
    
    for (int i = 0; i < 4; i++) {
      wave[i]->valid = i % 2; 
      wave[i]->filtid = i  * 0xAb + index; 
      wave[i]->threshold = i * 0x1234; 
      for (int j = 0; j < TSPIKEWAVE_LEN; j++) {
	wave[i]->wave[j] = index * 0x1000 + i * 0x100 + j; 
      }
    }
    
    return ts; 
    
  }
  
  void test_equality(const TSpike_t & t1, const TSpike_t & t2) 
  {

    BOOST_CHECK_EQUAL(t1.src, t2.src); 
    BOOST_CHECK_EQUAL(t1.time, t2.time); 
    
    BOOST_CHECK_EQUAL(t1.x.valid, t2.x.valid); 
    BOOST_CHECK_EQUAL(t1.y.valid, t2.y.valid); 
    BOOST_CHECK_EQUAL(t1.a.valid, t2.a.valid); 
    BOOST_CHECK_EQUAL(t1.b.valid, t2.b.valid); 

    BOOST_CHECK_EQUAL(t1.x.filtid, t2.x.filtid); 
    BOOST_CHECK_EQUAL(t1.y.filtid, t2.y.filtid); 
    BOOST_CHECK_EQUAL(t1.a.filtid, t2.a.filtid); 
    BOOST_CHECK_EQUAL(t1.b.filtid, t2.b.filtid); 

    BOOST_CHECK_EQUAL(t1.x.threshold, t2.x.threshold); 
    BOOST_CHECK_EQUAL(t1.y.threshold, t2.y.threshold); 
    BOOST_CHECK_EQUAL(t1.a.threshold, t2.a.threshold); 
    BOOST_CHECK_EQUAL(t1.b.threshold, t2.b.threshold); 

    for(int i = 0; i < TSPIKEWAVE_LEN; i++ ){ 
      BOOST_CHECK_EQUAL(t1.x.wave[i], t2.x.wave[i]); 
      BOOST_CHECK_EQUAL(t1.y.wave[i], t2.y.wave[i]); 
      BOOST_CHECK_EQUAL(t1.a.wave[i], t2.a.wave[i]); 
      BOOST_CHECK_EQUAL(t1.b.wave[i], t2.b.wave[i]); 
    }
    
  }

  Wave_t generateCanonicalWave(datasource_t src, int index) 
  {
    Wave_t wave; 
    wave.src = src; 
    wave.time = 0x1234 * (index + 7); 
    
    wave.sampratenum = index * 10 + 1; 
    wave.samprateden = index * 20 + 1; 
    wave.selchan = 10; 
    wave.filterid = src * index; 
    
    for(int i = 0; i < WAVEBUF_LEN; i++) {
      wave.wave[i] = index * 10 + src * 10000 + i; 
    }

    return wave; 
    
  }
  
  void test_equality(const Wave_t & w1, const Wave_t & w2) 
  {

    BOOST_CHECK_EQUAL(w1.src, w2.src); 
    BOOST_CHECK_EQUAL(w1.time, w2.time); 
    
  }

}

#ifndef SOMANETWORK_TEST_CANONICAL
#define SOMANETWORK_TEST_CANONICAL

#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   

#include <somanetwork/raw.h>
#include <somanetwork/tspike.h>
#include <somanetwork/wave.h>
#include <somanetwork/event.h>

/*
  Deterministic functions which generate canonical versions
  of the indicated packet type for debugging, indexed by "index"


 */ 

namespace somanetwork { 
  Raw_t generateCanonicalRaw(datasource_t src, int index); 
  void test_equality(const Raw_t & r1, const Raw_t & r2); 
  
  TSpike_t generateCanonicalTSpike(datasource_t src, int index); 
  void test_equality(const TSpike_t & r1, const TSpike_t & r2); 

  Wave_t generateCanonicalWave(datasource_t src, int index); 
  void test_equality(const Wave_t & r1, const Wave_t & r2); 

  EventList_t generateCanonicalEventList(int len, int index); 
  void test_equality(const EventList_t & el1, const EventList_t & el2); 
  
}

#endif

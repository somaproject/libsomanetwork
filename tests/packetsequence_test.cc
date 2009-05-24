

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "somanetwork/packetsequence.h"
#include <string>
using boost::unit_test::test_suite;
using namespace std;

BOOST_AUTO_TEST_SUITE(packetsequence)

BOOST_AUTO_TEST_CASE(test_simple)
{
  int SEQMAX = 100; 
  somanetwork::PacketSequence<string> ps("Hello", 1, SEQMAX); 
  BOOST_CHECK_EQUAL(ps.headID(), 1); 
  BOOST_CHECK_EQUAL(ps.headID(), ps.tailID()); 

  ps.append("Goodbye", 2); 
  BOOST_CHECK_EQUAL(ps.size(), 2); 
  BOOST_CHECK_EQUAL(ps.headID(), 1); 
  BOOST_CHECK_EQUAL(ps.tailID(), 2); 

  somanetwork::PacketSequence<string> ps2("Hello", 99, 100); 
  BOOST_CHECK_EQUAL(ps2.headID(), 99); 
  BOOST_CHECK_EQUAL(ps2.headID(), ps2.tailID()); 

  ps2.append("Goodbye", 0); 
  BOOST_CHECK_EQUAL(ps2.size(), 2); 
  BOOST_CHECK_EQUAL(ps2.headID(), 99); 
  BOOST_CHECK_EQUAL(ps2.tailID(), 0); 

}

BOOST_AUTO_TEST_CASE(test_errors)
{
  int SEQMAX = 100; 
  somanetwork::PacketSequence<string> ps("Hello", 1, 100); 
  BOOST_CHECK_THROW(ps.append("fail", 3), std::runtime_error); 
  ps.append("good", 2); 
  BOOST_CHECK_EQUAL(ps.size(), 2); 

  somanetwork::PacketSequence<string> ps2("Hello", 99, 100); 
  BOOST_CHECK_THROW(ps.append("fail", 100), std::runtime_error); 

}

BOOST_AUTO_TEST_CASE(contains)
{
  int SEQMAX = 100; 
  
  somanetwork::PacketSequence<string> ps("Hello", 1, 100); 
  for(int i = 2; i < 10; i++) {
    ps.append("Hello2", i); 
  }
  
  BOOST_CHECK(ps.contains(2)); 
  BOOST_CHECK(!ps.contains(12)); 

}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/utility.hpp>

#include "somanetwork/packetsequence.h"
#include "somanetwork/seqpktproto.h"
#include <string>
#include <boost/assign/std/list.hpp>
#include <boost/assert.hpp>

using boost::unit_test::test_suite;
using namespace std;
using namespace somanetwork; 
using namespace boost::assign; 


BOOST_AUTO_TEST_SUITE(seqpktproto)

typedef std::list<seqid_t> seqlist_t; 

SequentialPacketProtocol<string> * run_sequential_test(seqlist_t seqIDlist, seqlist_t seqIDlistRX, 
			 seqid_t SEQMAX)
{

  SequentialPacketProtocol<string> * spp = new SequentialPacketProtocol<string>(SEQMAX);
  
  // create the data and push into source
  std::list<std::string> strdata; 
  for (seqlist_t::iterator i = seqIDlist.begin(); i != seqIDlist.end(); ++i) {
    std::string val = boost::str(boost::format("Packet %d") % *i); 
    strdata.push_back(val); 
    spp->addPacket(val,*i); 
  }
  
  // receover the data
  SequentialPacketProtocol<string>::outqueue_t pkts = spp->getCompletedPackets(); 

  // compute correct data
  std::list<std::string>  correctstrdata; 
  for (seqlist_t::iterator i = seqIDlistRX.begin(); i != seqIDlistRX.end(); ++i) {
    std::string val = boost::str(boost::format("Packet %d") % *i); 
    correctstrdata.push_back(val); 
  }
  
  BOOST_CHECK_EQUAL(correctstrdata.size(), pkts.size()); 

  SequentialPacketProtocol<string>::outqueue_t::iterator recoveredPI = pkts.begin();  
  std::list<std::string>::iterator goodPI = correctstrdata.begin(); 
  
  while(goodPI != correctstrdata.end() and recoveredPI != pkts.end()) {
    BOOST_CHECK_EQUAL(*goodPI, *recoveredPI); 

    goodPI++; 
    recoveredPI++; 

  }
  return spp; 
}

BOOST_AUTO_TEST_CASE(seqid_math) {
  using namespace somanetwork; 

  BOOST_CHECK_EQUAL(seq_add(1, 1, 3), 2); 
  BOOST_CHECK_EQUAL(seq_add(1, 2, 3), 0); 
  BOOST_CHECK_EQUAL(seq_add(2, 2, 3), 1); 

  BOOST_CHECK_EQUAL(seq_dist(2, 2, 5), 0); 
  BOOST_CHECK_EQUAL(seq_dist(2, 1, 5), 1); 
  BOOST_CHECK_EQUAL(seq_dist(2, 0, 5), 2); 
  BOOST_CHECK_EQUAL(seq_dist(2, 4, 5), 3); 
  BOOST_CHECK_EQUAL(seq_dist(2, 3, 5), 4); 

  BOOST_CHECK_EQUAL(seq_sub(2, 5, 10), 7); 

}


BOOST_AUTO_TEST_CASE(test_inrange)
{

  BOOST_CHECK(somanetwork::inrange(0, 0, 5, 10)); 
  BOOST_CHECK(!somanetwork::inrange(6, 0, 5, 10)); 
  BOOST_CHECK_THROW(somanetwork::inrange(12, 0, 5, 10), std::runtime_error); 
  BOOST_CHECK_THROW(somanetwork::inrange(10, 0, 5, 10), std::runtime_error); 

  BOOST_CHECK(somanetwork::inrange(5, 0, 5, 10)); 
  BOOST_CHECK(!somanetwork::inrange(9, 0, 5, 10)); 

  BOOST_CHECK(somanetwork::inrange(8, 8, 2, 10)); 
  BOOST_CHECK(somanetwork::inrange(9, 8, 2, 10)); 
  BOOST_CHECK(somanetwork::inrange(0, 8, 2, 10)); 
  BOOST_CHECK(somanetwork::inrange(1, 8, 2, 10)); 
  BOOST_CHECK(somanetwork::inrange(2, 8, 2, 10)); 

}

BOOST_AUTO_TEST_CASE(test_inpastfuture)
{
  using namespace somanetwork; 
  seqid_t SEQMAX = 100; 
  
  SequentialPacketProtocol<string> spp(SEQMAX); 
  spp.addPacket("Hello", 10); 

  // check inFuture
  BOOST_CHECK(spp.inFuture(20)); 
  BOOST_CHECK(spp.inFuture(50)); 
  BOOST_CHECK(!spp.inFuture(63)); 
  BOOST_CHECK(!spp.inFuture(9)); 
  
  // check inPast

  BOOST_CHECK(spp.inPast(9)); 
  BOOST_CHECK(spp.inPast(0)); 
  BOOST_CHECK(spp.inPast(99)); 
  BOOST_CHECK(spp.inPast(75)); 
  BOOST_CHECK(spp.inPast(65)); 
  BOOST_CHECK(!spp.inPast(55)); 
  
  
}

BOOST_AUTO_TEST_CASE(test_lienar_addition) {
  /*
    Add a sequence of packets,
   */

  seqid_t SEQMAX = 4; 
  
  seqlist_t goodin; 
  goodin += 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3; 
  
  run_sequential_test(goodin, goodin, SEQMAX); 

}

BOOST_AUTO_TEST_CASE(test_out_of_order) {
  /*
    Add a sequence of packets with 
    one out of order
   */

  seqid_t SEQMAX = 20; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 5, 4, 6, 7, 8; 

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 4, 5, 6, 7, 8; 
  
  run_sequential_test(pktin, goodout, SEQMAX); 

}

BOOST_AUTO_TEST_CASE(test_out_of_order_with_wrap_around) {
  /*
    Add a sequence of packets with one out of order, 
    at the wrap-around interface
   */

  seqid_t SEQMAX = 12; 
  
  seqlist_t pktin; 
  pktin += 9, 10, 11, 1, 0, 2; 

  seqlist_t goodout; 
  goodout += 9, 10, 11, 0, 1, 2; 
  
  run_sequential_test(pktin, goodout, SEQMAX); 

  
}

BOOST_AUTO_TEST_CASE(test_dupe)
{

  seqid_t SEQMAX = 12; 
  
  seqlist_t pktin; 
  pktin += 1, 2, 3, 4, 4, 5, 6, 7, 6, 8; 

  seqlist_t goodout; 
  goodout += 1, 2, 3, 4, 5, 6, 7, 8; 
  
  run_sequential_test(pktin, goodout, SEQMAX); 


}

BOOST_AUTO_TEST_CASE(test_out_of_order_dupe)
{
  seqid_t SEQMAX = 12; 
  
  seqlist_t pktin; 
  pktin += 1, 2, 4, 2, 3, 2, 5, 6; 

  seqlist_t goodout; 
  goodout += 1, 2, 3, 4, 5, 6; 
  
  run_sequential_test(pktin, goodout, SEQMAX); 

}


BOOST_AUTO_TEST_CASE(test_retx)
{
  seqid_t SEQMAX = 12; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 5, 6, 7, 4, 8, 9, 10, 11;

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 
  std::list<seqid_t> retx =  spp->getRetransmitRequests(); 
  BOOST_CHECK_EQUAL(retx.front(), 4); 


}

BOOST_AUTO_TEST_CASE(test_retx_lost_single)
{
  /*
    See if we correctly issue a retx request and then give up
   */
  seqid_t SEQMAX = 25; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15;

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 
  std::list<seqid_t> retx =  spp->getRetransmitRequests(); 
  BOOST_CHECK_EQUAL(retx.front(), 4); 

}


BOOST_AUTO_TEST_CASE(test_retx_lost_multiple)
{
  /*
    See if we correctly issue multiple retx requests
    and then give up on them 
   */
  seqid_t SEQMAX = 25; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 
  std::list<seqid_t> retx =  spp->getRetransmitRequests(); 
  std::list<seqid_t>::iterator ri = retx.begin(); 

  BOOST_CHECK_EQUAL(*ri, 4); 
  ri++; 
  BOOST_CHECK_EQUAL(*ri, 5); 
  

}

BOOST_AUTO_TEST_CASE(test_random_permutation_packets)
{
  /*
    Randomly permute some packets and see if we recover
   */
  seqid_t SEQMAX = 25; 
  
  seqlist_t pktin; 
  pktin += 0, 3, 2, 1, 4, 6, 5, 7,  7, 9, 1, 2, 8; 

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 4, 5, 6, 7, 8, 9; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 
}

BOOST_AUTO_TEST_CASE(test_noise_packets)
{

  seqid_t SEQMAX = 1000; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 200,  4, 201, 202, 203, 5, 400,  6, 7, 8, 9; 

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 4, 5, 6, 7, 8, 9; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 

}


BOOST_AUTO_TEST_CASE(test_abort_sequence)
{

  seqid_t SEQMAX = 1000; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 100, 101, 102, 103, 104, 105, 106, 107, 108,
    109, 110, 111; 

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 100, 101, 102, 103, 104, 105, 106, 107, 108,
    109, 110, 111; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 

}

BOOST_AUTO_TEST_CASE(test_abort_sequence_2)
{

  seqid_t SEQMAX = 1000; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
              109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119; 

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
               109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 

}


BOOST_AUTO_TEST_CASE(test_abort_sequence_3)
{

  seqid_t SEQMAX = 1000; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 200, 201, 300, 301, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
              109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119; 

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
               109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119;
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 

}


BOOST_AUTO_TEST_CASE(test_overlapping_seq)
{
  seqid_t SEQMAX = 1000; 
  
  seqlist_t pktin; 
  pktin +=  0, 12, 1, 2, 3, 5, 6, 8, 4, 7, 8, 9, 10;

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 




}

BOOST_AUTO_TEST_CASE(test_abort_sequence_with_wraparound)
{

  seqid_t SEQMAX = 1000; 
  
  seqlist_t pktin; 
  pktin += 800, 801, 802, 803, 100, 101, 102, 900, 901, 
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14;

  seqlist_t goodout; 
  goodout += 800, 801, 802, 803, 900, 901, 
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 

}


BOOST_AUTO_TEST_CASE(stats_test_linear_addition) {
  /*
    Add a sequence of packets, and compute stats

   */

  seqid_t SEQMAX = 4; 
  
  seqlist_t goodin; 
  goodin += 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(goodin, goodin, SEQMAX); 

  SeqPacketProtoStats stats = spp->getStats(); 
  BOOST_CHECK_EQUAL(stats.rxPacketCount, 12); 
  BOOST_CHECK_EQUAL(stats.validPacketCount, 12); 
  BOOST_CHECK_EQUAL(stats.latestRXSequenceID, 3); 
  BOOST_CHECK_EQUAL(stats.currentSequenceID, 3); 
  BOOST_CHECK_EQUAL(stats.dupeCount, 0); 
  BOOST_CHECK_EQUAL(stats.lostCount, 0); 
  BOOST_CHECK_EQUAL(stats.retxReqCount, 0); 
  

}



BOOST_AUTO_TEST_CASE(stats_test) {
  /*
    Add a sequence of packets, and compute stats
    with some out of order and some dupes
   */

  seqid_t SEQMAX = 10; 
  
  seqlist_t goodin; 
  goodin += 0, 1, 2, 2, 3, 4, 5, 6, 7, 9, 0, 1, 8; 
  
  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1; 
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(goodin, goodout, SEQMAX); 

  SeqPacketProtoStats stats = spp->getStats(); 
  BOOST_CHECK_EQUAL(stats.rxPacketCount, 13); 
  BOOST_CHECK_EQUAL(stats.validPacketCount, 12); 
  BOOST_CHECK_EQUAL(stats.latestRXSequenceID, 8); 
  BOOST_CHECK_EQUAL(stats.currentSequenceID, 1); 
  BOOST_CHECK_EQUAL(stats.dupeCount, 1); 
  BOOST_CHECK_EQUAL(stats.lostCount, 0); 
  BOOST_CHECK_EQUAL(stats.retxReqCount, 1); 

  spp->resetStats(); 

  stats = spp->getStats(); 

  BOOST_CHECK_EQUAL(stats.rxPacketCount, 0); 
  BOOST_CHECK_EQUAL(stats.validPacketCount, 0); 
  BOOST_CHECK_EQUAL(stats.latestRXSequenceID, 8); 
  BOOST_CHECK_EQUAL(stats.currentSequenceID, 1); 
  BOOST_CHECK_EQUAL(stats.dupeCount, 0); 
  BOOST_CHECK_EQUAL(stats.lostCount, 0); 
  BOOST_CHECK_EQUAL(stats.retxReqCount, 0); 

  
}


BOOST_AUTO_TEST_CASE(stat_test_abort_sequence_3)
{

  seqid_t SEQMAX = 1000; 
  
  seqlist_t pktin; 
  pktin += 0, 1, 2, 3, 200, 201, 300, 301, 50, 100, 101, 
    102, 103, 104, 105, 106, 107, 108,
    109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119; 

  seqlist_t goodout; 
  goodout += 0, 1, 2, 3, 50, 100, 101, 102, 103, 104, 105, 106, 107, 108,
               109, 110, 111, 112, 113, 114, 115, 116,
               117, 118, 119;
  
  SequentialPacketProtocol<string> * spp = 
    run_sequential_test(pktin, goodout, SEQMAX); 

  SeqPacketProtoStats stats = spp->getStats(); 
  BOOST_CHECK_EQUAL(stats.rxPacketCount, 29); 
  BOOST_CHECK_EQUAL(stats.validPacketCount, 25); 
  BOOST_CHECK_EQUAL(stats.latestRXSequenceID, 119); 
  BOOST_CHECK_EQUAL(stats.currentSequenceID, 119); 
  BOOST_CHECK_EQUAL(stats.dupeCount, 0); 
  BOOST_CHECK_EQUAL(stats.lostCount, 95); 
  BOOST_CHECK_EQUAL(stats.retxReqCount, 16); 


}



BOOST_AUTO_TEST_SUITE_END()



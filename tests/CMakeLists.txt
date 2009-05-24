
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SIGC++_INCLUDE_DIRS})

SET(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")


add_library(network_test_utils
  canonical.cc
  fakeeventserver.cc
)

add_executable(tspiketest
  tspike_test.cc
  runner.cc)

add_executable(wavetest
  wave_test.cc
  runner.cc)

add_executable(rawtest
  raw_test.cc
  runner.cc)

add_executable(pktseqtest
  packetsequence_test.cc
  seqpktproto_test.cc
  runner.cc)

add_executable(datareceivertest
  datareceiver_test.cc
  tests.cc
  runner.cc)

add_executable(eventreceivertest
  eventreceiver_test.cc
  runner.cc)

add_executable(eventtxtest
  eventreceiver_test.cc
  eventtxtests.cc
  runner.cc)

add_executable(domainsockproxytest
  domainsockproxy_test.cc
  runner.cc)

set(testlist 
  tspiketest
  wavetest
  rawtest
  datareceivertest 
  eventreceivertest
  eventtxtest
  domainsockproxytest
  pktseqtest)

foreach(testname ${testlist})
  
  target_link_libraries(${testname}
    network_test_utils
    somanetwork)
  
  ADD_TEST(${testname} ${testname})

endforeach(testname)




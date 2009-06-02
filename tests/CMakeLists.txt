
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SIGC++_INCLUDE_DIRS})


add_library(network_test_utils
  canonical.cc
  fakeeventserver.cc
)

add_executable(tspiketest 
  test_config.h
  tspike_test.cc
  runner.cc)

add_executable(tinytest
  tiny_test.cc
  runner.cc)

add_executable(wavetest
  wave_test.cc
  runner.cc)

add_executable(networktest
  network_test.cc
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
  tinytest
  networktest
  domainsockproxytest
  pktseqtest)

SET(TEST_DATA_PATH ${PROJECT_SOURCE_DIR}/tests)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/tests/test_config.h.in
  ${PROJECT_BINARY_DIR}/tests/test_config.h)

# add the binary tree to the search path for include files
# so we will find this generated include file
include_directories("${PROJECT_BINARY_DIR}/tests")

foreach(testname ${testlist})
  
  target_link_libraries(${testname}
    network_test_utils
    somanetwork_test)
  
  ADD_TEST(${testname} ${testname})

endforeach(testname)



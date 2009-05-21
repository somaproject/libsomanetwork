
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SIGC++_INCLUDE_DIRS})

#TSpike Test

add_executable(tspiketest
  tspike_test.cc
  runner.cc)

add_executable(wavetest
  wave_test.cc
  runner.cc)

add_executable(rawtest
  raw_test.cc
  runner.cc)

add_executable(datareceivertest
  datareceiver_test.cc
  tests.cc
  runner.cc)

add_executable(eventreceivertest
  eventreceiver_test.cc
  eventtests.cc
  runner.cc)

add_executable(eventtxtest
  eventreceiver_test.cc
  eventtests.cc
  eventtxtests.cc
  runner.cc)

add_executable(pktseqtest
  packetsequence_test.cc
  seqpktproto_test.cc
  runner.cc)

set(testlist 
  tspiketest
  wavetest
  rawtest
  datareceivertest 
  eventreceivertest
  eventtxtest
  pktseqtest)

foreach(testname ${testlist})
  
  target_link_libraries(${testname}
    somanetworkstatic)
  
  ADD_TEST(${testname} ${testname})

endforeach(testname)




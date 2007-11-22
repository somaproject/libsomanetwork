AM_CXXFLAGS = $(BOOST_CPPFLAGS)  -I../src/
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_UNIT_TEST_FRAMEWORK_LIB)  $(BENCH_LIBS) ../src/libsomanetwork-1.0.la


check_PROGRAMS= alltest tspiketest wavetest datareceivertest eventreceivertest eventtxtest
alltest_SOURCES = tspike_test.cc wave_test.cc datareceiver_test.cc eventreceiver_test.cc runner.cc tests.cc eventtests.cc eventtxtests.cc eventtx_test.cc
tspiketest_SOURCES = tspike_test.cc runner.cc
wavetest_SOURCES = wave_test.cc runner.cc
datareceivertest_SOURCES = datareceiver_test.cc runner.cc tests.cc
eventreceivertest_SOURCES = eventreceiver_test.cc runner.cc eventtests.cc
eventtxtest_SOURCES = eventreceiver_test.cc runner.cc eventtests.cc eventtxtests.cc



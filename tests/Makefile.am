AM_CXXFLAGS = $(BOOST_CPPFLAGS)  -I../
AM_LDFLAGS = $(BOOST_LDFLAGS) \
	$(BOOST_THREAD_LDFLAGS) $(BOOST_THREAD_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_PROGRAM_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS) \
	$(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
	 $(BENCH_LIBS) 

LIBS = $(top_builddir)/somanetwork/libsomanetwork-1.0.la
TESTS=tspiketest wavetest rawtest datareceivertest eventreceivertest eventtxtest
# check_PROGRAMS= alltest tspiketest wavetest rawtest datareceivertest \
# 	eventreceivertest eventtxtest
check_PROGRAMS=  tspiketest wavetest rawtest datareceivertest eventreceivertest eventtxtest packetseqtest

# alltest_SOURCES = tspike_test.cc wave_test.cc raw_test.cc datareceiver_test.cc eventreceiver_test.cc runner.cc tests.cc eventtests.cc eventtxtests.cc eventtx_test.cc
# alltest_LDADD = $(top_builddir)/libsomanetwork-1.0.la

tspiketest_SOURCES = tspike_test.cc runner.cc
wavetest_SOURCES = wave_test.cc runner.cc
rawtest_SOURCES = raw_test.cc runner.cc
datareceivertest_SOURCES = datareceiver_test.cc runner.cc tests.cc
eventreceivertest_SOURCES = eventreceiver_test.cc runner.cc eventtests.cc
eventtxtest_SOURCES = eventreceiver_test.cc runner.cc eventtests.cc eventtxtests.cc
packetseqtest_SOURCES = packetsequence_test.cc seqpktproto_test.cc runner.cc



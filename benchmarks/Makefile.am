AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(BENCH_CFLAGS)  -I../src/
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_TEST_LDFLAGS)  $(BENCH_LIBS) ../src/libsomanetwork-1.0.la

#LIBS = $(BOOST_THREAD_LIBS) $(BOOST_TEST_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS)
LDADD = $(BOOST_THREAD_LIBS) $(BOOST_TEST_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS)

bin_PROGRAMS = soma-netbench  eventrx soma-netbench-tx
soma_netbench_SOURCES = glibbench.cc range.cc
eventrx_SOURCES = eventrx.cc 

soma_netbench_tx_SOURCES = synthtx.cc range.cc  eventsynthtx.cc \
	audiogen.cc digitaloutgen.cc periodicpattern.cc



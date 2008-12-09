AM_CXXFLAGS = $(BOOST_CPPFLAGS) $(BENCH_CFLAGS)  -I../src/
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_UNIT_TEST_FRAMEWORK_LIB)  $(BENCH_LIBS) ../src/libsomanetwork-1.0.la

bin_PROGRAMS = soma-netbench  eventrx soma-netbench-tx
soma_netbench_SOURCES = glibbench.cc range.cc
eventrx_SOURCES = eventrx.cc 

soma_netbench_tx_SOURCES = synthtx.cc range.cc  eventsynthtx.cc \
	audiogen.cc digitaloutgen.cc periodicpattern.cc

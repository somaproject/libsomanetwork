AM_CXXFLAGS = $(BOOST_CPPFLAGS) $(BENCH_CFLAGS)  -I../src/
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_UNIT_TEST_FRAMEWORK_LIB)  $(BENCH_LIBS) ../src/libsomanetwork-1.0.la

bin_PROGRAMS = glibbench  eventrx
glibbench_SOURCES = glibbench.cc
eventrx_SOURCES = eventrx.cc


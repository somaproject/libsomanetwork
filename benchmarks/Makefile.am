AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)  $(BENCH_LIBS)

noinst_PROGRAMS= glibbench
glibbench_SOURCES = glibbench.cc
glibbench_LDADD = ../src/libsomanetwork-1.0.la  
glibbench_CXXFLAGS = -I../src/  $(BENCH_CFLAGS)


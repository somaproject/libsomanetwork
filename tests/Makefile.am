# 
AM_CXXFLAGS = $(BOOST_CPPFLAGS)
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB) $(BOOST_UNIT_TEST_FRAMEWORK_LIB)

check_PROGRAMS= runner
runner_SOURCES = datareceiver_test.cc tests.cc runner.cc 
runner_LDADD = ../src/libsomanetwork-1.0.la
runner_CXXFLAGS = -I../src/

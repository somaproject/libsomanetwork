## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src benchmarks tests # tests benchmarks

AM_CXXFLAGS = $(BOOST_CPPFLAGS) $(FAKESRC_CFLAGS) $(BOOST_THREAD_CPPFLAGS) $(BOOST_PROGRAM_OPTIONS_CPPFLAGS) -pthread 
AM_LDFLAGS = $(BOOST_LDFLAGS) $(FAKESRC_LIBS) $(BOOST_THREAD_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_TEST_LDFLAGS)
LIBS = $(BOOST_THREAD_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = somanetwork-1.0.pc

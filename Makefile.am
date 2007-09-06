## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tests benchmarks

AM_CXXFLAGS = $(BOOST_CPPFLAGS) -pthread 
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)


pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = somanetwork-1.0.pc

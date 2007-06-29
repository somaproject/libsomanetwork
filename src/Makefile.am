# 

h_sources = datareceiver.h eventdispatcher.h eventreceiver.h eventsender.h network.h networkinterface.h packetreceiver.h ports.h tspipefifo.h

cc_sources = datareceiver.cc eventdispatcher.cc eventreceiver.cc eventsender.cc network.cc


library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(h_sources)

INCLUDES = -I$(top_srcdir)

lib_LTLIBRARIES= libsomanetwork-1.0.la
libexamplelib_1_0_la_SOURCES= $(h_sources) $(cc_sources)
libexamplelib_1_0_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)

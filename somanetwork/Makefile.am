# 
h_sources = datareceiver.h eventdispatcher.h eventreceiver.h eventsender.h network.h \
	networkinterface.h packetreceiver.h ports.h tspipefifo.h seqpktproto.h packetsequence.h

cc_sources = datareceiver.cc eventdispatcher.cc eventreceiver.cc eventsender.cc \
	 network.cc fakenetwork.cc tspike.cc wave.cc event.cc raw.cc


library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = network.h event.h eventtx.h datapacket.h tspike.h networkinterface.h \
	fakenetwork.h datareceiver.h eventreceiver.h eventsender.h packetreceiver.h \
	eventdispatcher.h ports.h tspipefifo.h wave.h tspike.h raw.h seqpktproto.h packetsequence.h

lib_LTLIBRARIES= libsomanetwork-1.0.la
libsomanetwork_1_0_la_SOURCES= $(h_sources) $(cc_sources)
libsomanetwork_1_0_la_CXXFLAGS= $(FAKESRC_CFLAGS) -Werror
libsomanetwork_1_0_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
libsomanetwork_1_0_la_LIBS = $(BOOST_THREAD_LIBS)

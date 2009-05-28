set(datatype_SRCS
raw.cc
wave.cc
tspike.cc
)

set(somanetwork_SRCS 
datareceiver.cc
soma_event.cc
eventdispatcher.cc
eventreceiver.cc
eventsender.cc
fakenetwork.cc
network.cc
netsockproxy.cc
domainsockproxy.cc
${datatype_SRCS}
)

set(somanetwork_HEADERS
datapacket.h
sockproxy.h
netsockproxy.h
datareceiver.h
eventdispatcher.h
soma_event.h
eventreceiver.h
eventsender.h
eventtx.h
fakenetwork.h
lossydatareceiver.h
network.h
networkinterface.h
packetreceiver.h
packetsequence.h
ports.h
raw.h
seqpktproto.h
tspike.h
tspipefifo.h
wave.h
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SIGC++_INCLUDE_DIRS})
include_directories(${Event_INCLUDE_DIRS})

ADD_LIBRARY(somanetwork SHARED
  ${somanetwork_SRCS} 
  ${somanetwork_HEADERS})

target_link_libraries(somanetwork 
  ${Boost_LIBRARIES} ${SIGC++_LIBRARIES} event ${Event_LIBRARIES})

SET_TARGET_PROPERTIES(somanetwork
  PROPERTIES VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_SOVERSION})

install( TARGETS somanetwork LIBRARY
  DESTINATION lib)

install(FILES ${somanetwork_HEADERS}
  DESTINATION
  include/${PROJECT_NAME}-${LIBRARY_VERSION}/${PROJECT_NAME})

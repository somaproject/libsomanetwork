
set(datatype_SRCS
  raw.cc
  tspike.cc
  wave.cc)

set(somanetwork_SRCS 
  datareceiver.cc
  event.cc
  eventtx.cc
  eventdispatcher.cc
  eventreceiver.cc
  eventsender.cc
  fakenetwork.cc
  network.cc
  netsockproxy.cc
  domainsockproxy.cc
  logging.cc
  ${datatype_SRCS})

set(somanetwork_HEADERS
datapacket.h
sockproxy.h
netsockproxy.h
datareceiver.h
eventdispatcher.h
event.h
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
logging.h

)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${SIGC++_INCLUDE_DIRS})

ADD_LIBRARY(somanetwork SHARED
  ${somanetwork_SRCS})

target_link_libraries(somanetwork 
  ${Boost_LIBRARIES} ${SIGC++_LIBRARIES})

ADD_LIBRARY(somanetwork_test
  ${somanetwork_SRCS})

target_link_libraries(somanetwork_test
  ${Boost_LIBRARIES} ${SIGC++_LIBRARIES})

SET_TARGET_PROPERTIES(somanetwork
  PROPERTIES VERSION ${LIBRARY_VERSION}
  SOVERSION ${LIBRARY_SOVERSION})

install( TARGETS somanetwork LIBRARY
  DESTINATION lib)

install(FILES ${somanetwork_HEADERS}
  DESTINATION
  include/${PROJECT_NAME}-${LIBRARY_VERSION}/${PROJECT_NAME})

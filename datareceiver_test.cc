#include <boost/test/unit_test.hpp>

#include "datareceiver.h"
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <asio.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

using asio::ip::udp;

using boost::unit_test::test_suite;

std::list<RawData*> rawDataBuffer; 

void rawDataReceiver(RawData* rd) {
  rawDataBuffer.push_back(rd); 
  
}

void finish(const asio::error_code& /*e*/, asio::io_service * ios )
{
  ios->stop(); 
}

char * fakeDataPacket(unsigned int seq, char src, char typ)
{
  char * data = new char[BUFSIZE]; 
  unsigned int seqh = htonl(seq); 
  memcpy(data, &seqh,  4); 
  data[4] = src; 
  data[5] = typ; 

  return data; 

}

BOOST_AUTO_TEST_CASE( simpletest )
{
  // There's an interesting question of what we want to test here
  // and how we do it. We need to hook up datareceiver to the dispatcher, 
  // and then hook up something that sends datagrams, and hook up a callback

  // and then test them all. 

  // first, set up the generic IO 
  asio::io_service io_service;

  // construct tx endpoint

  udp::endpoint receiver_endpoint(asio::ip::address::from_string("127.0.0.1"),
				  4000); 

  udp::socket socket(io_service);
  socket.open(udp::v4());
  
  // construct buffer

  
  // then, construct the receiver
  DataReceiver dr(io_service, 0, 0, &rawDataReceiver); 

  // send packets:
  char * fd = fakeDataPacket(0, 0, 0); 
  socket.send_to(asio::buffer(fd, BUFSIZE), receiver_endpoint);  
  
  // set up timer to finish

  asio::deadline_timer t(io_service, boost::posix_time::seconds(5));
  t.async_wait(boost::bind(finish,
			   asio::placeholders::error, &io_service));

  // run service

  io_service.run();

  delete[] fd; 

  
  BOOST_CHECK_EQUAL(rawDataBuffer.empty(),  false); 
  rawDataBuffer.erase(rawDataBuffer.begin(), rawDataBuffer.end()); 

}

BOOST_AUTO_TEST_CASE( seqtest )
{

  // first, set up the generic IO 
  asio::io_service io_service;



  // construct tx endpoint

  udp::endpoint receiver_endpoint(asio::ip::address::from_string("127.0.0.1"),
				  4000); 

  udp::socket socket(io_service);
  socket.open(udp::v4());
  
  
  // empty output buffer
  rawDataBuffer.erase(rawDataBuffer.begin(), rawDataBuffer.end()); 

  // then, construct the receiver
  DataReceiver dr(io_service, 0, 0, &rawDataReceiver); 

  // send packets:
  for (int i = 0; i < 4; i++) {
    char * fd = fakeDataPacket(i, 0, 0); 
    socket.send_to(asio::buffer(fd, BUFSIZE), receiver_endpoint);  
  }
  // set up timer to finish

  asio::deadline_timer t(io_service, boost::posix_time::seconds(5));
  t.async_wait(boost::bind(finish,
			   asio::placeholders::error, &io_service));

  // run service

  io_service.run();

  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 4); 
}

BOOST_AUTO_TEST_CASE( reversetest )
{

  // first, set up the generic IO 
  asio::io_service io_service;

  // construct tx endpoint

  udp::endpoint receiver_endpoint(asio::ip::address::from_string("127.0.0.1"),
				  4000); 

  udp::socket socket(io_service);
  socket.open(udp::v4());
  
  
  // empty output buffer
  rawDataBuffer.erase(rawDataBuffer.begin(), rawDataBuffer.end()); 

  // then, construct the receiver
  DataReceiver dr(io_service, 0, 0, &rawDataReceiver); 

  // send packets:
  char * fd = fakeDataPacket(0, 0, 0); 
  socket.send_to(asio::buffer(fd, BUFSIZE), receiver_endpoint);  

  for (int i = 0; i < 4; i++) {
    char * fd = fakeDataPacket(4-i, 0, 0); 
    socket.send_to(asio::buffer(fd, BUFSIZE), receiver_endpoint);  
  }
  // set up timer to finish

  asio::deadline_timer t(io_service, boost::posix_time::seconds(5));
  t.async_wait(boost::bind(finish,
			   asio::placeholders::error, &io_service));

  // run service

  io_service.run();

  
  BOOST_CHECK_EQUAL(rawDataBuffer.size(), 4); 
}

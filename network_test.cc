#include <boost/test/unit_test.hpp>
#include "datareceiver.h"
#include "network.h" 
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <asio.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

using asio::ip::udp;

using boost::unit_test::test_suite;

char * fakeDataPacket(unsigned int seq, char src, char typ)
{
  char * data = new char[BUFSIZE]; 
  unsigned int seqh = htonl(seq); 
  memcpy(data, &seqh,  4); 
  data[4] = src; 
  data[5] = typ; 

  return data; 

}

void sendPackets(udp::socket & socket,
		 int seqstart, int seqstop, int src, int typ)
{
  // send packets:
  udp::endpoint receiver_endpoint(asio::ip::address::from_string("127.0.0.1"),
				  dataPortLookup(typ, src)); 

  for (int i = seqstart; i < seqstop+ 1; i++) {
    char * fd = fakeDataPacket(i, src, typ); 
    socket.send_to(asio::buffer(fd, BUFSIZE), receiver_endpoint);  
  }
}


BOOST_AUTO_TEST_CASE( simpletest )
{
  Network network; 
  network.enableDataRx(0, 0);
  network.enableDataRx(0, 1);
  network.enableDataRx(0, 2);

  network.run(); 

  // first, set up the generic IO 
  asio::io_service io_service;

  // construct tx endpoint


  udp::socket socket(io_service);
  socket.open(udp::v4());
  
  int N = 11; 
  sendPackets(socket, 0, 10, 0, 0); 
  sendPackets(socket, 0, 0, 0, 1); 
  sendPackets(socket, 0, 10, 0, 2); 
  sendPackets(socket, 1, 10, 0, 1); 

  for (int i = 0; i < N*3; i++) 
    {
      char x; 
      read(network.getTSPipeFifoPipe(), &x, 1); 
      //int y = tp.pop(); 
      //BOOST_CHECK_EQUAL(y, i); 
    }

  network.shutdown(); 


  BOOST_CHECK_EQUAL(4, 4); 
}

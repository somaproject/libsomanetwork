#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <asio.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <somanetwork/datareceiver.h>
#include <somanetwork/network.h>
#include "tests.h"

using asio::ip::udp;

using boost::unit_test::test_suite;


class ReTxServer
{
public:
  ReTxServer(asio::io_service& io_service)
    : socket_(io_service, udp::endpoint(udp::v4(), 4400))
  {
    start_receive();
  }

private:
  void start_receive()
  {
    socket_.async_receive_from(
        asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&ReTxServer::handle_receive, this,
          asio::placeholders::error,
          asio::placeholders::bytes_transferred));
  }

  void handle_receive(const asio::error_code& error,
      std::size_t /*bytes_transferred*/)
  {
    if (!error || error == asio::error::message_size)
      {
	
	unsigned char typ = recv_buffer_[0]; 
	unsigned char src = recv_buffer_[1]; 
	unsigned int seq; 
	memcpy(&seq, &recv_buffer_[2], 4); 
	unsigned int seq_host; 
	seq_host = ntohl(seq); 
	
	char * resp = fakeDataPacket(seq_host, src, typ); 
	udp::endpoint retxep = remote_endpoint_;
	retxep.port(dataPortLookup(typ, src)); 
	
	socket_.async_send_to(asio::buffer(resp, BUFSIZE), 
			      retxep,
			      boost::bind(&ReTxServer::handle_send, 
					  this, resp,
					  asio::placeholders::error,
					  asio::placeholders::bytes_transferred));
	
	start_receive();
      }
  }
  
  void handle_send(char * message,
		   const asio::error_code& /*error*/,
		   std::size_t /*bytes_transferred*/)
  {
    delete message; 
  }

  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  boost::array<char, 6> recv_buffer_;
};

void sendPackets(udp::socket & socket,
		 int seqstart, int seqstop, int src, int typ)
{
  // send packets:
  udp::endpoint receiver_endpoint(asio::ip::address::from_string("127.0.0.1"),
				  dataPortLookup(typ, src)); 

  for (int i = seqstart; i <= seqstop; i++) {
    char * fd = fakeDataPacket(i, src, typ); 

    socket.send_to(asio::buffer(fd, BUFSIZE), receiver_endpoint);  
  }
}


// retx response


BOOST_AUTO_TEST_CASE( simpletest )
{
  Network network; 
  network.enableDataRx(0, 0);
  network.enableDataRx(1, 0);
  network.enableDataRx(2, 0);

  network.run(); 

  // first, set up the generic IO 
  asio::io_service io_service;

  // construct tx endpoint


  udp::socket socket(io_service);
  socket.open(udp::v4());
  
  int N = 11; 
  sendPackets(socket, 0, 10, 0, 0); 
  sendPackets(socket, 0, 0, 1, 0); 
  sendPackets(socket, 0, 10, 2, 0); 
  sendPackets(socket, 1, 10, 1, 0); 
  
  std::vector<sequence_t> rxseqpos(3); 
  rxseqpos[0] = 0; rxseqpos[1] = 0; rxseqpos[2] = 0; 

  for (int i = 0; i < N*3; i++) 
    {
      char x; 
      read(network.getTSPipeFifoPipe(), &x, 1); 
      RawData * rdp = network.getNewData(); 
      
      BOOST_CHECK_EQUAL(rxseqpos[rdp->src], rdp->seq); 
      rxseqpos[rdp->src]++; 

    }

  network.shutdown(); 


}

// BOOST_AUTO_TEST_CASE( simpletest2 )
// {
//   Network network; 
//   network.enableDataRx(0, 0);
//   network.enableDataRx(0, 1);
//   network.enableDataRx(0, 2);

//   network.run(); 

//   // first, set up the generic IO 
//   asio::io_service io_service;

//   // construct tx endpoint

//   udp::socket socket(io_service);
//   socket.open(udp::v4());
  
//   int N = 11; 
//   sendPackets(socket, 0, 10, 0, 0); 
//   sendPackets(socket, 0, 0, 0, 1); 
//   sendPackets(socket, 0, 10, 0, 2); 
//   sendPackets(socket, 1, 10, 0, 1); 

//   for (int i = 0; i < N*3; i++) 
//     {
//       char x; 
//       read(network.getTSPipeFifoPipe(), &x, 1); 
//       //int y = tp.pop(); 
//       //BOOST_CHECK_EQUAL(y, i); 
//     }

//   network.shutdown(); 


//   BOOST_CHECK_EQUAL(4, 4); 
// }



BOOST_AUTO_TEST_CASE( simpleReTX )
{
  Network network; 
  network.enableDataRx(0, 0);
  network.enableDataRx(1, 0);
  network.enableDataRx(2, 0);

  network.run(); 


  // create the retx thread
  asio::io_service retx_service;
  ReTxServer retxserver(retx_service);
  boost::thread retxthread(boost::bind(&asio::io_service::run, &retx_service));


  // construct tx endpoint

  // first, set up the generic IO 
  asio::io_service io_service;


  udp::socket socket(io_service);
  socket.open(udp::v4());
  
  int N = 11; 
  sendPackets(socket, 0, 10, 0, 0); 
  sendPackets(socket, 0, 5, 1, 0); 
  sendPackets(socket, 0, 10, 2, 0); 
  sendPackets(socket, 8, 10, 1, 0); 
  //sendPackets(socket, 6, 6, 1, 0); 

  for (int i = 0; i < N*3; i++) 
    {
      char x; 
      read(network.getTSPipeFifoPipe(), &x, 1); 
      RawData* rdp  = network.getNewData(); 
      
      //BOOST_CHECK_EQUAL(y, i); 
    }

  network.shutdown();
  retx_service.stop(); 
  retxthread.join(); 

  BOOST_CHECK_EQUAL(4, 4); 
}

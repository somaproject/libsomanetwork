#include <iostream>
#include <boost/array.hpp>
#include <asio.hpp>

using asio::ip::udp;

int main() 
{
    asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::endpoint receiver_endpoint(asio::ip::address::from_string("127.0.0.1"),
				    4000); 

    udp::socket socket(io_service);
    socket.open(udp::v4());

    boost::array<char, 600> send_buf; 
    for (int i = 0; i < 100000; i++) {
      socket.send_to(asio::buffer(send_buf), receiver_endpoint);
    }

}

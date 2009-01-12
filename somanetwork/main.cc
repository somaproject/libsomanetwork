#include <iostream> 
#include <asio.hpp>
#include <boost/bind.hpp>
#include "datareceiver.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unistd.h>
#include <vector>
#include <list>
#include <sys/time.h>
#include <time.h>
#include <map>
#include <boost/program_options.hpp>

using asio::ip::udp;
namespace po = boost::program_options;

void pushback(RawData * rd)
{
  
}

void print(const asio::error_code & e, 
	   asio::deadline_timer* t, 
	   std::list<DataReceiver*>  * ds)
{
  std::list<DataReceiver* >::iterator i; 
//   for (i = ds->begin(); i != ds->end(); i++) {
//     int tus = ((*i)->lastPacket_ - (*i)->firstPacket_).total_microseconds(); 

//     std::cout << "Received  " << (*i)->getBufferSize() << " in "
// 	      << (*i)->lastPacket_ - (*i)->firstPacket_ << "("
// 	      << ( (*i)->getBufferSize()) / (float)tus * 1e6 
// 	      << " packets/sec) " <<     std::endl; 
//   }  
  exit(0); 

}

std::list<DataReceiver* > dataServers; 
 
int main(int argc, char * argv[])
{

  //parse command options
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("startchan", po::value<int>()->default_value(0), 
     "first channel to receive for")
    ("endchan", po::value<int>()->default_value(0), 
     "last channel to receive for")
    ("disableretx", po::value<int>()->default_value(0),
     "Request packet retransmission, 1=yes"); 

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  int N = vm["endchan"].as<int>() -  vm["startchan"].as<int>() + 1; 

  try
  {
    asio::io_service io_service;
    for (int i = vm["startchan"].as<int>(); 
	 i <= vm["endchan"].as<int>(); i++) {
      DataReceiver* server = new DataReceiver(io_service, 0, i,
					      pushback);
      dataServers.push_back(server); 

    }
    // completion timer at 10 seconds
    asio::deadline_timer t(io_service, boost::posix_time::seconds(100));
    t.async_wait(boost::bind(print,
			     asio::placeholders::error, &t, &dataServers)); 

    io_service.run();
  }

  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

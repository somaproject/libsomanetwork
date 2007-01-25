#include <iostream>
#include "network.h"
#include <boost/program_options.hpp>

/*

Here, we :
1. create the network object
2. run until we've received a total of N packets
3. stop and measure how long it took. 


*/
namespace po = boost::program_options;


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
     "Request packet retransmission, 1=yes")
    ("packetcount", po::value<int>()->default_value(1000),
     "Wait for this many packets total"); 

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  int N = vm["endchan"].as<int>() -  vm["startchan"].as<int>() + 1; 
  
  std::vector<int> rxCnt(N); 
  
  try
  {
    Network network; 
    for (int i = vm["startchan"].as<int>(); 
	 i <= vm["endchan"].as<int>(); i++) {
      network.enableDataRx(i, 0); // always assume type 0
      //rxCnt[i - vm["startchan"].as<int>()] = -1; 
    }
    network.run(); 

    // create timers
    ptime t1(neg_infin); 
    
    
    for (int i = 0; i < vm["packetcount"].as<int>(); i++) {
      char x; 
      read(network.getTSPipeFifoPipe(), &x, 1); 
      if (t1.is_neg_infinity()) 
	{
	  t1 = microsec_clock::local_time(); 
	}
      
      RawData * rdp = network.getNewData(); 
      char chan = rdp->src; 
      if (chan >= vm["startchan"].as<int>() and
	  chan <= vm["endchan"].as<int>()) 
	{
	  rxCnt[chan - vm["startchan"].as<int>()]++; 
	} else { 
	  std::cerr << "Received packet for unwanted channel" << std::endl; 
	}
    }
    ptime t2(microsec_clock::local_time()); 
    network.shutdown(); 
 
    for (int i = vm["startchan"].as<int>(); 
	 i <= vm["endchan"].as<int>(); i++) {
      int cnt = rxCnt[i - vm["startchan"].as<int>()]; 
      std::cout << "channel " << i  << " : " << 
	float(cnt) / (t2 -t1).total_microseconds()*1e6 
		<< " packets / second" << std::endl;
      
    }
    
    
  }
  
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

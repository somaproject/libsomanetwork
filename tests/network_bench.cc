#include <iostream>
#include "network.h"
#include <boost/program_options.hpp>

#include <unistd.h>     /* standard unix functions, like getpid()       */
#include <sys/types.h>  /* various type definitions, like pid_t         */
#include <signal.h>     /* signal name macros, and the kill() prototype */


/*

Here, we :
1. create the network object
2. run until we've received a total of N packets
3. stop and measure how long it took. 


*/
namespace po = boost::program_options;


Network *  network; 

void printDataStats()
{

  std::vector<DataReceiverStats> drs = network->getDataStats(); 
  std::vector<DataReceiverStats>::iterator i = drs.begin(); 
  for (i = drs.begin(); i != drs.end(); i++ ) 
    {
      std::cout << "src = " << i->source << "typ = " << i->type << std::endl;
      std::cout << "    " << "pktcount=" << i->pktCount 
		<< " latestSeq=" << i->latestSeq 
		<< " dupeCount=" << i->dupeCount
		<< " pendingCount=" << i->pendingCount 
		<< " mispktcnt = " << i->missingPacketCount 
		<< " reTxRxCount = " << i->reTxRxCount 
		<< " outOfOrderCount = " << i->outOfOrderCount 
		<< std::endl; 
    }
  

}

void catch_int(int sig_num)
{
  /* re-set the signal handler again to catch_int, for next time */
  signal(SIGINT, catch_int);
  /* and print the message */
  printDataStats(); 

  network->shutdown(); 
  
  exit(0); 
}


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
    network =  new Network(); 
    for (int i = vm["startchan"].as<int>(); 
	 i <= vm["endchan"].as<int>(); i++) {
      network->enableDataRX(i, 0); // always assume type 0
      //rxCnt[i - vm["startchan"].as<int>()] = -1; 
    }
    network->run(); 

    // create timers
    ptime t1(neg_infin); 

    signal(SIGINT, catch_int);
    
    for (int i = 0; i < vm["packetcount"].as<int>(); i++) {
      char x; 
      read(network->getTSPipeFifoPipe(), &x, 1); 
      if (t1.is_neg_infinity()) 
	{
	  t1 = microsec_clock::local_time(); 
	}
      
      DataPacket_t * rdp = network->getNewData(); 
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
    network->shutdown(); 
 
    for (int i = vm["startchan"].as<int>(); 
	 i <= vm["endchan"].as<int>(); i++) {
      int cnt = rxCnt[i - vm["startchan"].as<int>()]; 
      std::cout << "channel " << i  << " : " << 
	float(cnt) / (t2 -t1).total_microseconds()*1e6 
		<< " packets / second" << std::endl;
      
    }
    printDataStats();     
    
  }
  
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

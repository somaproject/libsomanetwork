#include <iostream>
#include <somanetwork/network.h>
#include <somanetwork/tspike.h>
#include <list>

std::string SOMAIP = "10.0.0.2"; 

int main()
{
  // Create the soma network object
  somanetwork::pNetworkInterface_t pnet = 
    somanetwork::Network::createINet(SOMAIP); 
  
  // enable TSPIKE from data source 0
  pnet->enableDataRX(0, somanetwork::TSPIKE); 

  // start running
  pnet->run(); 
  
  int datapipe = pnet->getDataFifoPipe(); 
  
  // create a list to hold our spikes
  std::list<somanetwork::TSpike_t> tspike_list; 

  // now read 10 packets
  for (int i = 0; i < 10; i++) {
    char dummychar; 
    read(datapipe, &dummychar, 1);  // read a single byte out of the fifo, or block
    
    somanetwork::pDataPacket_t dp = pnet->getNewData(); 
    
    // now convert to a tspike packet

    somanetwork::TSpike_t ts = rawToTSpike(dp); 
    
    // save the tspike packet for later
    tspike_list.push_back(ts); 
  }

  pnet->shutdown(); 

  std::cout << "We received " << tspike_list.size() 
	    << " tspikes" << std::endl; 

}

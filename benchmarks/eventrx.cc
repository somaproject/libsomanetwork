#include <iostream>
#include <list>
#include <network.h>
#include <map>
#include <boost/program_options.hpp>
#include "network.h"


typedef std::map<eventcmd_t, int> eventmap_t; 


int main(void)
{

  std::string SOMAIP("10.0.0.2"); 

  Network somanetwork(SOMAIP); 
  std::list < EventList_t * > pell; 

  std::cout << "Network object created" << std::endl; 
  // now, try and get events: 
  
  int eventpipe = somanetwork.getEventFifoPipe(); 
  somanetwork.run(); 


  int PKTCNT = 100000; 
  for (int i = 0; i < PKTCNT; i++) {
    char dummy; 
    read(eventpipe, &dummy, 1);  // will block
    pell.push_back(somanetwork.getNewEvents()); 
    
  }
  somanetwork.shutdown(); 
  EventReceiverStats ers = somanetwork.getEventStats(); 
  
  std::cout << "Finished recieving " << PKTCNT << " event packets" 
	    << std::endl; 

  std::cout << "--------------------------------------------------------"
	    << std::endl; 

  std::cout << "Event stats:" << std::endl; 
  std::cout << "--------------------------------------------------------"
	    << std::endl; 


  std::cout << "pktCount = " << ers.pktCount << std::endl; 
  std::cout << "latestSeq = " << ers.latestSeq << std::endl; 
  std::cout << "dupeCount = " << ers.dupeCount << std::endl; 
  std::cout << "pendingCount = " << ers.pendingCount 
	    << std::endl; 
  std::cout << "missingPacketCount = " 
	    << ers.missingPacketCount << std::endl; 
  std::cout << "reTxRxCount = " << ers.reTxRxCount << std::endl; 
  std::cout << "outOfOrderCount = " << ers.outOfOrderCount << std::endl; 


  std::cout << "--------------------------------------------------------"
	    << std::endl; 


  std::cout << "That's " << pell.size() << " event lists " << std::endl; 
  
  std::list < EventList_t * >::iterator pel; 
  
  eventmap_t eventmap; 

  for (pel = pell.begin(); pel != pell.end(); pel++) {
    EventList_t::iterator pe; 
    for (pe = (*pel)->begin(); pe != (*pel)->end(); pe++)
      {
	Event_t evt = *pe; 
	if (eventmap.find(evt.cmd) != eventmap.end() ){
	  // this exists; inc; 
	  eventmap[evt.cmd] += 1; 
	} else {
	  // need to insert
	  eventmap[evt.cmd] = 1; 
	}
      }
  }
  
  std::cout << "event command frequency" << std::endl; 
  // print the results:
  for (eventmap_t::iterator em = eventmap.begin(); 
       em != eventmap.end(); em++)
    {
      std::cout << (int) em->first << ' ' << (int) em->second <<  std::endl; 

    }

}

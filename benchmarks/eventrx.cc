#include <iostream>
#include <list>
#include <somanetwork/network.h>
#include <map>
#include <boost/program_options.hpp>

using namespace somanetwork; 

typedef std::map<eventcmd_t, int> eventmap_t; 

int main(void)
{

  std::string SOMAIP("10.0.0.2"); 

  Network somanetwork(SOMAIP); 
  std::list < pEventPacket_t > pell; 

  std::cout << "Network object created" << std::endl; 
  // now, try and get events: 
  
  int eventpipe = somanetwork.getEventFifoPipe(); 
  somanetwork.run(); 


  int PKTCNT = 10000; 
  for (int i = 0; i < PKTCNT; i++) {
    char dummy; 
    read(eventpipe, &dummy, 1);  // will block
    pell.push_back(somanetwork.getNewEvents()); 
    
  }
  somanetwork.shutdown(); 
  SeqPacketProtoStats ers = somanetwork.getEventStats(); 
  
  std::cout << "Finished recieving " << PKTCNT << " event packets" 
	    << std::endl; 

  std::cout << "--------------------------------------------------------"
	    << std::endl; 

  std::cout << "Event stats:" << std::endl; 
  std::cout << "--------------------------------------------------------"
	    << std::endl; 


  std::cout << "rxPacketCount = " << ers.rxPacketCount << std::endl; 
  std::cout << "validPacketCount = " << ers.validPacketCount << std::endl; 
  std::cout << "latestRXSequenceID = " << ers.latestRXSequenceID << std::endl; 
  std::cout << "currentSequenceID = " << ers.currentSequenceID << std::endl; 
  std::cout << "dupeCount = " << ers.dupeCount << std::endl; 
  std::cout << "listCount = " << ers.lostCount << std::endl; 
  std::cout << "retxReqCount = " << ers.retxReqCount << std::endl; 

  std::cout << "--------------------------------------------------------"
	    << std::endl; 


  std::cout << "That's " << pell.size() << " event lists " << std::endl; 
  
  std::list < pEventPacket_t >::iterator pel; 
  
  eventmap_t eventmap; 

//   for (pel = pell.begin(); pel != pell.end(); pel++) {
//     EventList_t::iterator pe; 
//     for (pe = (*pel)->begin(); pe != (*pel)->end(); pe++)
//       {
// 	Event_t evt = *pe; 
// 	if (eventmap.find(evt.cmd) != eventmap.end() ){
// 	  // this exists; inc; 
// 	  eventmap[evt.cmd] += 1; 
// 	} else {
// 	  // need to insert
// 	  eventmap[evt.cmd] = 1; 
// 	}
//       }
//   }
  
//   std::cout << "event command frequency" << std::endl; 
//   // print the results:
//   for (eventmap_t::iterator em = eventmap.begin(); 
//        em != eventmap.end(); em++)
//     {
//       std::cout << (int) em->first << ' ' << (int) em->second <<  std::endl; 

//     }

}

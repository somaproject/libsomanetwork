#include <boost/format.hpp>
#include "eventtx.h"

namespace somanetwork { 

  std::ostream& operator<< (std::ostream &out, const EventTX_t &event)
  {
    out << "eventtx:"; 
    
    bool all = true; 
    std::list<eventsource_t> dest; 
    for (int i = 0; i < ADDRBITS; i++) {
      if (event.destaddr[i] ) {
	dest.push_back(i); 
      } else {
	all = false; 
      }
    }
    
    if (all) {
      out << " dest: bcast all "; 
    } else {
      out << " dest: "; 
      for (std::list<eventsource_t>::iterator i = dest.begin(); 
	   i != dest.end(); ++i) {
	out << boost::format("%02.2X ") % (int)(*i); 
      }
    }
    out << "(" <<  event.event << ")" ; 
    return out; 
    
  }

  std::ostream& operator<< (std::ostream &out, const EventTXList_t &eventlist)
  {
    for(EventTXList_t::const_iterator ei = eventlist.begin(); 
	ei != eventlist.end(); ++ei) {
      out << *ei;
    }
    return out; 


  }

}

#ifndef EVENT_H
#define EVENT_H
#include <stdint.h>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <byteswap.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
#include <list>
#include <string.h>

typedef uint8_t eventsource_t ; 
typedef uint8_t eventcmd_t; 
typedef uint32_t eventseq_t; 
const int EVENTLEN = 6; 

const int EBUFSIZE = 1550; 

class Event_t
{
 public:
  eventcmd_t cmd; 
  eventsource_t src; 
  boost::array<uint16_t, EVENTLEN-1> data;
  Event_t() {
    for (int i = 0; i < EVENTLEN-1; ++i) {
      data[i] = 0; 
    }
  }
}; 


typedef std::vector<Event_t> EventList_t; 
typedef boost::shared_ptr<EventList_t> pEventList_t; 

struct EventPacket_t
{
  eventseq_t seq; 
  bool missing; 
  pEventList_t events; 
}; 

typedef boost::shared_ptr<EventPacket_t> pEventPacket_t; 

pEventPacket_t newEventPacket(boost::array<char, EBUFSIZE> buffer, 
			      size_t len);

std::vector<char> createEventBuffer(int seq, 
				    std::list<EventList_t> els); 

#endif // EVENT_H

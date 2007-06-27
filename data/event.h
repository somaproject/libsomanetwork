#ifndef EVENT_H
#define EVENT_H
#include <stdint.h>

#include <boost/array.hpp>
#include <byteswap.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
#include <list>

typedef uint8_t eventsource_t ; 
typedef uint8_t eventcmd_t; 
typedef uint32_t eventseq_t; 
const int EVENTLEN = 6; 

const int EBUFSIZE = 1550; 

struct Event_t
{
  eventcmd_t cmd; 
  eventsource_t src; 
  boost::array<uint16_t, EVENTLEN-1> data;
}; 


typedef std::vector<Event_t> EventList_t; 

struct EventPacket_t
{
  eventseq_t seq; 
  bool missing; 
  EventList_t * events; 
}; 


inline EventPacket_t * newEventPacket(boost::array<char, EBUFSIZE> buffer, 
				      size_t len) 
{
  // We need to take in the len because the total number of event
  // packets is a function of the length of this packet; 
  EventPacket_t *  pepkt = new EventPacket_t; 
  pepkt->missing = false; 

  pepkt->seq = ntohl(*((int *) &buffer[0])); 
  int remaininglen = len -4; 

  // decode the transmited event set into an array of events 
  EventList_t * pEventList = new EventList_t(); 
  int maxevents = (remaininglen) / 12; 

  pEventList->reserve(maxevents); 
  
  size_t bpos = 4; 
  
  while ((bpos+1) < len) 
    {
      uint16_t neventlen, eventlen; 
      memcpy(&neventlen, &buffer[bpos], sizeof(neventlen)); 
      eventlen = ntohs(neventlen); 
      bpos += 2; 
      for (int evtnum = 0; evtnum < eventlen; evtnum++)
	{
	  // extract out individual events
	  Event_t evt; 
	  evt.cmd = buffer[bpos]; 
	  bpos++; 

	  evt.src = buffer[bpos]; 
	  bpos++; 

	  memcpy(&evt.data[0], &buffer[bpos], EVENTLEN*sizeof(uint16_t)); 
	  bpos += (EVENTLEN-1)*sizeof(uint16_t); 
	  pEventList->push_back(evt); 
	}
    }
  pepkt->events = pEventList; 

  return pepkt; 
}

inline std::vector<char> createEventBuffer(int seq, 
					   std::list<EventList_t> els)
{
  // we really need better names for a lot of these functions, such
  // as this one
  // we take in a sequence number and a list of event lists, and 
  // generate a comparable packet, which we should
  // then be able to decode
  //  
  // we return a std::vector because we need to be able to process
  // the variable-length components. 
  
  int totallen = 4; 
  for(std::list<EventList_t>::iterator i = els.begin(); 
      i != els.end(); 
      i++)
    {
      totallen += 2;  // eventlen length
      totallen += i->size()*12; 
    }
  
  std::vector<char> buffer(totallen); 
  
  // write the sequence number
  eventseq_t nseq = htonl(seq); 
  memcpy(&buffer[0], &nseq, 4); 
  int bpos = 4; 
  for(std::list<EventList_t>::iterator i = els.begin(); 
      i != els.end(); 
      i++)
    {
      uint16_t neventlen = htons(i->size()); 
      memcpy(&buffer[bpos], &neventlen, sizeof(neventlen));
      bpos += 2; 
      for (EventList_t::iterator pevt = i->begin(); 
	   pevt != i->end(); pevt++)
	{
	  memcpy(&buffer[bpos], &(pevt->cmd), 1); 
	  bpos++; 

	  memcpy(&buffer[bpos], &(pevt->src), 1); 
	  bpos++; 

	  memcpy(&buffer[bpos], &(pevt->data[0]), 10); 
	  bpos += 10; 
	}
      
    }
  return buffer; 
}
#endif // EVENT_H

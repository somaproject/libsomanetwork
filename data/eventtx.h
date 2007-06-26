#ifndef EVENTTX_H
#define EVENTTX_H

#include <vector>

#include "event.h"


const int ADDRBITS = 80; 
const int ADDRSHORTS = 5; 

typedef uint16_t eventtxnonce_t; 

struct EventTX_t
{
  boost::array<bool, ADDRBITS> destaddr; 
  Event_t event; 
}; 

typedef std::vector<EventTX_t> EventTXList_t; 

inline std::vector<char> createEventTXBuffer(eventtxnonce_t nonce, 
					     const EventTXList_t & events)
{
  // create a buffer by marshalling the event list and nonce
  
  // compute total buffer length:
  int totallen = sizeof(eventtxnonce_t) + 2 + 16*2*events.size(); 
  std::vector<char> buffer(totallen); 

  int bpos = 0; 
  
  // write the nonce
  eventtxnonce_t nnonce = htons(nonce); 
  memcpy(&buffer[bpos], &nnonce, sizeof(eventtxnonce_t)); 
  bpos += sizeof(nonce); 

  //write the event count

  uint16_t nEventCount = htons(events.size()); 
  memcpy(&buffer[bpos], &nEventCount, sizeof(uint16_t)); 
  bpos += sizeof(nEventCount); 

  // now the actual events:
  for (EventTXList_t::const_iterator e = events.begin(); 
       e != events.end(); ++e)
    {
      // first copy the addresses in a slightly odd order
      for ( int i = 0; i < ADDRSHORTS; i++)
	{
	  uint16_t addrblock = 0; 
	  for (int b = 0; b < 16; b++)
	    {
	      if(e->destaddr[i*16 + 15 - b]){
		addrblock |= 1; 
	      }
	      addrblock = addrblock << 1; 
	    }
	  uint16_t naddrblock = htons(addrblock); 
	  memcpy(&buffer[bpos], &naddrblock, sizeof(naddrblock)); 
	  bpos += sizeof(naddrblock); 
	  
	}
      
      
      // now the event data
      buffer[bpos] = e->event.cmd; 
      bpos++; 
      
      buffer[bpos] = e->event.src; 
      bpos++;
      
      for (int i = 0; i < 5; i++)
	{
	  uint16_t neventword = htons(e->event.data[i]); 
	  memcpy(&buffer[bpos], &neventword, sizeof(neventword)); 
	  bpos += sizeof(neventword); 
	  
	}
      
      // now five words of padding
      for (int i = 0; i <5 ; i++) {
	uint16_t pad = 0;
	memcpy(&buffer[bpos], &pad, sizeof(pad));
	bpos += sizeof(pad);
      }
      
    }
  
  return buffer; 

}
	
// now, extract an eventtx list from a packet on the wire; for debugging, 
// to verify this is an identity operation

inline eventtxnonce_t getEventListFromBuffer(const std::vector<char> & buffer,
					     EventTXList_t * pEmptyEventList)
{
  // returns the discovered events in pEmptyEventList and the nonce as
  // the return value. 

  assert(pEmptyEventList->size() == 0); 

  int bpos = 0; 

  eventtxnonce_t nnonce, hnonce; 
  memcpy(&nnonce, &buffer[bpos], sizeof(nnonce)); 
  hnonce = ntohs(nnonce); 
  bpos += sizeof(nnonce); 

  // event count
  uint16_t necnt, hecnt; 
  memcpy(&necnt, &buffer[bpos], sizeof(necnt)); 
  bpos += sizeof(necnt); 
  hecnt = ntohs(necnt); 

  // now, extract out the events!
  for (int i = 0; i < hecnt; i++)
    {
      EventTX_t eventtx; 
      // first, the addresses
      for ( int i = 0; i < ADDRSHORTS; i++)
	{
	  uint16_t haddrblock(0), naddrblock(0); 
	  
	  memcpy(&naddrblock, &buffer[bpos], sizeof(naddrblock)); 
	  bpos += sizeof(naddrblock); 

	  haddrblock = ntohs(naddrblock); 
	  for (int b = 0; b < 16; b++)
	    {
	      eventtx.destaddr[i*16+b] = false; 

	      if (haddrblock & 0x01) {
		eventtx.destaddr[i*16 + b] = true; 
	      }
	      
	      haddrblock = haddrblock >> 1; 
	    }
	}
      
      // then extract out the event data
      eventtx.event.cmd = buffer[bpos]; 
      bpos++; 

      eventtx.event.src = buffer[bpos]; 
      bpos++;

      for (int i = 0; i < 5; i++)
	{
	  uint16_t neventword(0), heventword(0);
	  memcpy(&neventword, &buffer[bpos], sizeof(neventword)); 
	  bpos += sizeof(heventword); 

	  heventword = ntohs(neventword); 
	  eventtx.event.data[i] = heventword; 
	}
      
      pEmptyEventList->push_back(eventtx); 
      bpos += 5*2;  // padding
      


    }
  return hnonce; 

}

#endif //EVENTTX_H

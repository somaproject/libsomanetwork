#ifndef WAVE_TYPE_T
#define WAVE_TYPE_T

#include <stdint.h>
#include <iostream>
#include "datapacket.h"
#include <byteswap.h>
#include <arpa/inet.h>
#include <wave>

/* 

Type declaration for Data packet

*/ 

struct Wave_t
{
  uint8_t src; 
  uint64_t time; 
  uint16_t samprate; 
  uint16_t selchan; 
  uint16_t filterid; 
  std::vector<int32_t> wave; 

}; 

inline
Wave_t rawToWave(const DataPacket_t * dp)
{
  Wave_t w; 

  if (! dp->missing) {
    w.src = rd->body[1]; 

    // extract out len
    uint16_t nlen, hlen; 
    memcpy(&nlen, &rd->body[2], sizeof(nlen)); 
    hlen = ntohs(nlen); 

    // extract out selected channel
    uint16_t nchan, hchan; 
    memcpy(&nchan, &rd->body[4], sizeof(nchan)); 
    hchan = ntohs(nchan); 
    w.selchan = hchan; 

    // extract out the sampling rate (only an integer)
    uint16_t nsamp, hsamp; 
    memcpy(&nsamp, &rd->body[6], sizeof(samp)); 
    hsamp = ntohs(nsamp); 
    w.samprate = hsamp; 

    // extract out the time
    uint64_t ntime, htime; 
    memcpy(&ntime, &rd->body[8], sizeof(ntime)); 
    htime = ntohll(ntime); 
    w.time = htime; 

    w.reserve(hlen);
    
    size_t bpos = 8+8; 
    
    for (int i = 0; i < hlen; i++ )
      {
	int32_t x; 
	memcpy(&x, &rd->body[bpos], sizeof(x)); 
	bpos += 4; 
	w[i] = ntohl(x); 
      }
    
  } else {
    std::cout << "recovered spike is missing" << std::endl; 

  }
  
  return w; 


}

inline DataPacket_t * rawFromWave(const Wave_t & w)
{
  
  DataPacket_t * rdp = new DataPacket_t; 
  
  rdp->src = w.src; 
  rdp->type = WAVE; 
  rdp->seq = 0; 
  rdp->missing = false; 

  // initial header word
  rdp->body[0] = datatypeToChar(WAVE); 
  rdp->body[1] = w.src; 

  // input packet len
  hlen = w.wave.size(); 
  uint16_t nlen, hlen; 
  nlen = htons(hlen); 
  memcpy(&rdp->body[2], &nlen, sizeof(nlen)); 
  
  // extract out selected channel
  uint16_t nchan, hchan; 
  hchan = w.selchan; 
  nchan = htons(hchan); 
  memcpy(&rd->body[4], &nchan, sizeof(nchan)); 

  // extract out the sampling rate (only an integer)
  hsamp = w.samprate; 
  uint16_t nsamp, hsamp; 
  nsamp = htons(hsamp); 
  memcpy(&rd->body[6], &nsamp,  sizeof(nsamp)); 

  
  // extract out the time
  htime = w.time; 
  uint64_t ntime, htime; 
  ntime = ntohll(htime); 
  memcpy(&rd->body[8], &ntime, sizeof(ntime)); 

  // put in the primary packet data

  size_t bpos = 8+8; 
  
  for (int i = 0; i < hlen; i++ )
    {
      int32_t x;
      x = htonl(w.wave[i]); 
      memcpy(&rd->body[bpos], &x, sizeof(x)); 
      bpos += 4; 
      w[i] = htonl(x); 
    }

  return rdp; 

}

#ifndef TSPIKE_TYPE_H
#define TSPIKE_TYPE_H
#include <stdint.h>
#include <netdata/rawdata.h>
#include <byteswap.h>
#include <arpa/inet.h>

/*
Type declaration for Tetrode Spike data packet.



*/

const int TSPIKEWAVE_LEN = 32; 

struct TSpikeWave_t {
  uint8_t filtid; 
  uint8_t valid; 
  int32_t threshold; 
  int32_t wave[TSPIKEWAVE_LEN]; 
}; 


struct TSpike_t
{
  uint8_t src; 
  uint64_t time; 
  TSpikeWave_t x; 
  TSpikeWave_t y; 
  TSpikeWave_t a; 
  TSpikeWave_t b; 

}; 

inline 
TSpike_t rawToTSpike(const RawData * rd)
{
  if (! rd->missing) {
    TSpike_t ts; 
    ts.src = rd->body[1]; 
    
    uint64_t time; 
    memcpy(&time,  &(rd->body[4]), 8); 

    ts.time = htonll(time); 
    TSpikeWave_t * ptrs[] = {&ts.x, &ts.y, &ts.a, &ts.b}; 
    size_t bpos = (size_t) &rd->body[12]; 
    for (int i = 0; i < 4; i++)
      {
	TSpikeWave_t * tsp= ptrs[i]; 
	tsp->filtid = *((uint8_t*)bpos); 
	bpos++; 
	
	tsp->valid = *((uint8_t*)bpos); 
	bpos++; 
	
	tsp->threshold = ntohl(*((uint32_t *)bpos)); 
	bpos += sizeof(tsp->threshold); 
	for(int j = 0; j < TSPIKEWAVE_LEN; j++)
	  {
	    tsp->wave[j] = ntohl(*((uint32_t *)bpos)); 
	    bpos += sizeof(tsp->wave[0]); 

	  }
	
	
      }
    return ts; 
  }

}

inline RawData * rawFromTSpike(const TSpike_t & ts)
{
  
  RawData * rdp = new RawData; 
  rdp->src = ts.src; 
  rdp->seq = 0; 
  rdp->body[0] = 0; 
  rdp->body[1] = ts.src;
  rdp->body[2] = 0; 
  rdp->body[3] = 0; 
  uint64_t htime = htonll(ts.time); 
  memcpy((void*)(&rdp->body[4]), &htime, sizeof(htime)); 
  

  const TSpikeWave_t * ptrs[] = {&ts.x, &ts.y, &ts.a, &ts.b}; 
  size_t bpos = (size_t) &rdp->body[12]; 
  for (int i = 0; i < 4; i++)
    {
      const TSpikeWave_t * tswp = ptrs[i]; 
      memcpy((void*)bpos, &tswp->filtid, 1); 
      bpos++; 
      memcpy((void*)bpos, &tswp->valid, 1); 
      bpos++; 
	
      int32_t nthreshold = htonl(tswp->threshold); 
      memcpy((void*)bpos, &nthreshold, sizeof(nthreshold)); 
      bpos += sizeof(tswp->threshold); 

      for (int i = 0; i < TSPIKEWAVE_LEN; i++)
	{
	  int32_t x = htonl(tswp->wave[i]);
	  memcpy((void*)bpos, &x, sizeof(x)); 
	  bpos += sizeof(x); 
	}
    }
  
  return  rdp; 

}

#endif // TSPIKE_H_H

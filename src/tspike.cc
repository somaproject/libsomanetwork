#include "tspike.h"

TSpike_t rawToTSpike(pDataPacket_t rd)
{
  /* 
     We examine a datapacket's body and extract out all associated data
     necessary to recreate the TSpike
  */

  TSpike_t ts; 
  if (! rd->missing) {

    ts.src = rd->body[1]; 
    
    uint64_t time; 
    memcpy(&time,  &(rd->body[4]), 8); 

    ts.time = ntohll(time); 
    TSpikeWave_t * ptrs[] = {&ts.x, &ts.y, &ts.a, &ts.b}; 
    size_t bpos = (size_t) &rd->body[12]; 
    for (int i = 0; i < 4; i++)
      {
	TSpikeWave_t * tsp= ptrs[i]; 
	bpos += 2; // FIXME need to decode valid and srcchan

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

  } else {
    std::cout << "recovered spike is missing!" << std::endl;
  }

  return ts; 

}

pDataPacket_t rawFromTSpike(const TSpike_t & ts)
{
  /*
    Take a TSpike and construct an associated DataPacket. 
    the seq field is set to zero. 

  */ 
  
  pDataPacket_t rdp(new DataPacket_t); 
  rdp->src = ts.src; 
  rdp->typ = TSPIKE; 
  rdp->seq = 0; 
  rdp->missing = false; 
  rdp->body[0] = datatypeToChar(TSPIKE);  
  rdp->body[1] = ts.src;
  rdp->body[2] = 0; 
  rdp->body[3] = 0; 
  uint64_t htime = htonll(ts.time); 
  bzero(&(rdp->body[0]), BUFSIZE-HDRLEN); 
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

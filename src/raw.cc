#include "raw.h"

Raw_t rawToRaw(pDataPacket_t dp)
{
  Raw_t raw; 

  if (! dp->missing) {
    raw.src = dp->body[1]; 
    
    uint64_t time; 
    memcpy(&time, &(dp->body[2]), 8); 
    raw.time = ntohll(time); 

    // extract out selected channel
    uint16_t nchan, hchan; 
    memcpy(&nchan, &dp->body[10], sizeof(nchan)); 
    hchan = ntohs(nchan); 
    raw.chansrc = hchan; 

    // extract out the filterid
    uint32_t nfilterid, hfilterid; 
    memcpy(&nfilterid, &dp->body[12], sizeof(nfilterid)); 
    hfilterid = ntohl(nfilterid); 
    raw.filterid = hfilterid; 

    size_t bpos = 16; 
    
    for (int i = 0; i < RAWBUF_LEN; i++ )
      {
	int32_t x; 
	memcpy(&x, &dp->body[bpos], sizeof(x)); 
	bpos += 4; 
	raw.data[i] = ntohl(x); 
      }
    
  } else {
    std::cout << "recovered raw packet is missing" << std::endl; 

  }
  
  return raw; 

}

pDataPacket_t rawFromRaw(const Raw_t & raw)
{
  
  pDataPacket_t rdp(new DataPacket_t); 
  
  rdp->src = raw.src;
  rdp->typ = RAW;
  rdp->seq = 0;
  rdp->missing = false;

  // zero packet
  bzero(&(rdp->body[0]), BUFSIZE-HDRLEN); 

  // initial header word
  rdp->body[0] = datatypeToChar(RAW);
  rdp->body[1] = raw.src;


  // put in the time
  uint64_t ntime, htime;
  htime = raw.time;
  ntime = ntohll(htime);
  memcpy(&rdp->body[2], &ntime, sizeof(ntime));

  // the target channel
  uint16_t nchan, hchan;
  hchan = raw.chansrc; 
  nchan = htons(hchan);
  memcpy(&(rdp->body[10]), &nchan, sizeof(nchan));

  //  write the filterid
  uint32_t nfilterid, hfilterid;
  hfilterid = raw.filterid;
  nfilterid = htonl(hfilterid);
  memcpy(&rdp->body[12], &nfilterid,  sizeof(nfilterid));

  // put in the primary packet data

  size_t bpos = 16; 
  
  for (int i = 0; i < RAWBUF_LEN; i++ )
    {
      int32_t x;
      x = htonl(raw.data[i]);
      memcpy(&rdp->body[bpos], &x, sizeof(x));
      bpos += sizeof(x);
    }

  return rdp; 

}

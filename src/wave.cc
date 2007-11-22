#include "wave.h"

Wave_t rawToWave(pDataPacket_t dp)
{
  Wave_t w; 

  if (! dp->missing) {
    w.src = dp->body[1]; 

    // extract out len
    uint16_t nlen, hlen; 
    memcpy(&nlen, &dp->body[2], sizeof(nlen)); 
    hlen = ntohs(nlen); 

    if (hlen > WAVEBUF_LEN) { 
      hlen = WAVEBUF_LEN; 
    }

    // extract out selected channel
    uint16_t nchan, hchan; 
    memcpy(&nchan, &dp->body[4], sizeof(nchan)); 
    hchan = ntohs(nchan); 
    w.selchan = hchan; 

    // extract out the sampling rate (only an integer)
    uint16_t nsamp, hsamp; 
    memcpy(&nsamp, &dp->body[6], sizeof(hsamp)); 
    hsamp = ntohs(nsamp); 
    w.samprate = hsamp; 

    // extract out the filterid
    uint16_t nfilterid, hfilterid; 
    memcpy(&nfilterid, &dp->body[8], sizeof(nfilterid)); 
    hfilterid = ntohs(nfilterid); 
    w.filterid = hfilterid; 

    // extract out the time
    uint64_t ntime, htime; 
    memcpy(&ntime, &dp->body[10], sizeof(ntime)); 
    htime = ntohll(ntime); 
    w.time = htime; 

    size_t bpos = 10+8; 
    
    
    for (int i = 0; i < hlen; i++ )
      {
	int32_t x; 
	memcpy(&x, &dp->body[bpos], sizeof(x)); 
	bpos += 4; 
	w.wave[i] = ntohl(x); 
      }
    
  } else {
    std::cout << "recovered spike is missing" << std::endl; 

  }
  
  return w; 


}

pDataPacket_t rawFromWave(const Wave_t & w)
{
  
  pDataPacket_t rdp(new DataPacket_t); 
  
  rdp->src = w.src;
  rdp->typ = WAVE;
  rdp->seq = 0;
  rdp->missing = false;

  // zero packet
  bzero(&(rdp->body[0]), BUFSIZE-HDRLEN); 
  // initial header word
  rdp->body[0] = datatypeToChar(WAVE);
  rdp->body[1] = w.src;

  // input packet len
  uint16_t nlen, hlen;
  hlen = WAVEBUF_LEN;
  nlen = htons(hlen);
  memcpy(&rdp->body[2], &nlen, sizeof(nlen));
  
  // extract out selected channel
  uint16_t nchan, hchan;
  hchan = w.selchan;
  nchan = htons(hchan);
  memcpy(&(rdp->body[4]), &nchan, sizeof(nchan));

  // extract out the sampling rate (only an integer)
  uint16_t nsamp, hsamp;
  hsamp = w.samprate;
  nsamp = htons(hsamp);
  memcpy(&rdp->body[6], &nsamp,  sizeof(nsamp));

  // extract out the filter id
  uint16_t nfilterid, hfilterid;
  hfilterid = w.filterid;
  nfilterid = htons(hfilterid);
  memcpy(&rdp->body[8], &nfilterid,  sizeof(nfilterid));

   
  // extract out the time
  uint64_t ntime, htime;
  htime = w.time;
  ntime = ntohll(htime);
  memcpy(&rdp->body[10], &ntime, sizeof(ntime));

  // put in the primary packet data

  size_t bpos = 10+8; 
  
  for (int i = 0; i < hlen; i++ )
    {
      int32_t x;
      x = htonl(w.wave[i]);
      memcpy(&rdp->body[bpos], &x, sizeof(x));
      bpos += sizeof(x);
    }

  return rdp; 

}

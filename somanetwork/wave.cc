#include "wave.h"

namespace somanetwork {

Wave_t rawToWave(pDataPacket_t dp)
{
  Wave_t w; 

  if (! dp->missing) {
    w.src = dp->body[1]; 


    // extract out the time
    uint64_t ntime, htime; 
    memcpy(&ntime, &dp->body[2], sizeof(ntime)); 
    htime = ntohll(ntime); 
    w.time = htime; 

    // extract out version
    uint16_t nversion, hversion; 
    memcpy(&nversion, &dp->body[10], sizeof(nversion)); 
    hversion = ntohs(nversion); 

    // extract out the sampling rate numerator
    uint32_t nsampnum, hsampnum; 
    memcpy(&nsampnum, &dp->body[12], sizeof(hsampnum)); 
    hsampnum = ntohl(nsampnum); 
    w.sampratenum = hsampnum; 

    // extract out the sampling rate denominator
    uint32_t nsampden, hsampden; 
    memcpy(&nsampden, &dp->body[16], sizeof(hsampden)); 
    hsampden = ntohl(nsampden); 
    w.samprateden = hsampden; 

    // extract out the filterid
    uint32_t nfilterid, hfilterid; 
    memcpy(&nfilterid, &dp->body[20], sizeof(nfilterid)); 
    hfilterid = ntohl(nfilterid); 
    w.filterid = hfilterid; 

    uint16_t nchansrc, hchansrc; 
    memcpy(&nchansrc, &dp->body[24], sizeof(nchansrc)); 
    hchansrc = ntohs(nchansrc); 
    w.selchan = hchansrc; 

    size_t bpos = 26; 
    
    
    for (int i = 0; i < WAVEBUF_LEN; i++ )
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

  
  // extract out the time
  uint64_t ntime, htime;
  htime = w.time;
  ntime = ntohll(htime);
  memcpy(&rdp->body[2], &ntime, sizeof(ntime));

  // extract out version
  uint16_t nversion, hversion;
  hversion = 0x100; 
  nversion = htons(hversion);
  memcpy(&(rdp->body[10]), &nversion, sizeof(nversion));

  // extract out the sampling rate (only an integer)
  uint32_t nsampnum, hsampnum;
  hsampnum = w.sampratenum;
  nsampnum = htonl(hsampnum);
  memcpy(&rdp->body[12], &nsampnum,  sizeof(nsampnum));

  // extract out the sampling rate (only an integer)
  uint32_t nsampden, hsampden;
  hsampden = w.samprateden;
  nsampden = htonl(hsampden);
  memcpy(&rdp->body[16], &nsampden,  sizeof(nsampden));

  // extract out the filter id
  uint32_t nfilterid, hfilterid;
  hfilterid = w.filterid;
  nfilterid = htonl(hfilterid);
  memcpy(&rdp->body[20], &nfilterid,  sizeof(nfilterid));

  // extract out selected channel
  uint16_t nchan, hchan;
  hchan = w.selchan;
  nchan = htons(hchan);
  memcpy(&(rdp->body[24]), &nchan, sizeof(nchan));
  // put in the primary packet data

  size_t bpos = 26; 
  
  for (int i = 0; i < WAVEBUF_LEN; i++ )
    {
      int32_t x;
      x = htonl(w.wave[i]);
      memcpy(&rdp->body[bpos], &x, sizeof(x));
      bpos += sizeof(x);
    }

  return rdp; 

}

}

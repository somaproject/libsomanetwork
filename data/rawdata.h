#ifndef RAWDATA_H
#define RAWDATA_H

#include <boost/array.hpp>
#include <byteswap.h>

const int BUFSIZE = 1024; 
const int HDRLEN = 6;

typedef unsigned char datasource_t; 
typedef unsigned char datatype_t; 
typedef unsigned int sequence_t; 

#define ntohll(x) bswap_64(x)
#define htonll(x) bswap_64(x)

struct RawData
{
  sequence_t seq; 
  unsigned char src; 
  unsigned char typ; 
  bool missing; 
  boost::array<char, BUFSIZE - HDRLEN> body;
}; 


inline RawData * newRawData(boost::array<char, BUFSIZE> buffer) 
{
  RawData * prd = new RawData; 
    
  prd->seq = ntohl(*((int *) &buffer[0])); 
  prd->typ = buffer[4]; 
  prd->src = buffer[5]; 
  prd->missing = false; 

  for(int i = HDRLEN; i < BUFSIZE; i++) {
    prd->body[i - HDRLEN] = buffer[i]; 
  }

  return prd; 
}

inline int dataPortLookup(int type, int source) {
  return 4000  + type*64 + source;  
}
#endif // 

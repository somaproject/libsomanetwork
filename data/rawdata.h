#ifndef RAWDATA_H
#define RAWDATA_H

#include <boost/array.hpp>
#include <byteswap.h>
#include <arpa/inet.h>
#include <netinet/in.h>


const int BUFSIZE = 1024; 
const int HDRLEN = 6;

typedef unsigned char datasource_t; 
enum datatype_t {TSPIKE, WAVE, RAW};

inline char datatypeToChar(datatype_t x)
{
  switch (x)
    {
    case TSPIKE:
      return 0; 
    case WAVE:
      return 1;
    case RAW:
      return 2; 
    default:
      return 0; 
    }; 
}

inline datatype_t charToDatatype(char x)
{
  switch(x)
    { 
    case 0:
      return TSPIKE; 
    case 1:
      return WAVE; 
    case 2:
      return RAW; 
    default:
      return TSPIKE; 
    }
}

typedef unsigned int sequence_t; 

#define ntohll(x) bswap_64(x)
#define htonll(x) bswap_64(x)

struct DataPacket_t
{
  sequence_t seq; 
  datasource_t src; 
  datatype_t typ; 
  bool missing; 
  boost::array<char, BUFSIZE - HDRLEN> body;
}; 


inline DataPacket_t * newDataPacket(boost::array<char, BUFSIZE> buffer) 
{
  DataPacket_t * prd = new DataPacket_t; 
    
  prd->seq = ntohl(*((int *) &buffer[0])); 
  prd->typ = charToDatatype(buffer[4]); 
  prd->src = buffer[5]; 
  prd->missing = false; 

  for(int i = HDRLEN; i < BUFSIZE; i++) {
    prd->body[i - HDRLEN] = buffer[i]; 
  }

  return prd; 
}

#endif // 

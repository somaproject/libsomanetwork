#ifndef RAWDATA_H
#define RAWDATA_H

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <byteswap.h>
#include <arpa/inet.h>
#include <netinet/in.h>


const int BUFSIZE = 1024; 
const int HDRLEN = 4;

typedef unsigned char datasource_t; 
enum datatype_t {TSPIKE = 0, 
		 WAVE = 1, 
		 RAW = 2};

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

typedef boost::shared_ptr<DataPacket_t> pDataPacket_t; 

inline
pDataPacket_t newDataPacket(boost::array<char, BUFSIZE> buffer) 
{
  /* 
     This function takes in a raw UDP packet off the wire (as a buffer)
     and extracts out the header data, including:
        sequence ID
	type
	source
	
     It then copies the frame minus sequence header into 
     the "body" field
     
  */


  pDataPacket_t prd(new DataPacket_t); 
    
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

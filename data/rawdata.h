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

#endif // 

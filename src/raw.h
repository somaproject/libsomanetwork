#ifndef RAW_TYPE_T
#define RAW_TYPE_T

#include <stdint.h>
#include <iostream>
#include "datapacket.h"
#include <byteswap.h>
#include <arpa/inet.h>

/* 

Type declaration for Raw packet

*/ 

namespace somanetwork { 
const int RAWBUF_LEN = 128; 

struct Raw_t
{
  uint8_t src; 
  uint64_t time; 
  uint16_t chansrc; 
  uint32_t filterid; 
  int32_t data[RAWBUF_LEN]; 

}; 

Raw_t rawToRaw(pDataPacket_t dp); 
pDataPacket_t rawFromRaw(const Raw_t & w); 
}

#endif // RAW_TYPE_H

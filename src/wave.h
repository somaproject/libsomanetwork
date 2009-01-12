#ifndef WAVE_TYPE_T
#define WAVE_TYPE_T

#include <stdint.h>
#include <iostream>
#include "datapacket.h"
#include <byteswap.h>
#include <arpa/inet.h>

/* 

Type declaration for Data packet

*/ 

namespace somanetwork { 
const int WAVEBUF_LEN = 128; 

struct Wave_t
{
  uint8_t src; 
  uint64_t time; 
  uint16_t sampratenum; 
  uint16_t samprateden; 
  uint16_t selchan; 
  uint32_t filterid; 
  int32_t wave[WAVEBUF_LEN]; 

}; 

Wave_t rawToWave(pDataPacket_t dp); 
pDataPacket_t rawFromWave(const Wave_t & w); 
} 

#endif // WAVE_TYPE_H

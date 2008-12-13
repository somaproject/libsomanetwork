#ifndef TSPIKE_TYPE_H
#define TSPIKE_TYPE_H
#include <stdint.h>
#include <iostream>
#include "datapacket.h"
#include <byteswap.h>
#include <arpa/inet.h>
/*
Type declaration for Tetrode Spike data packet.

*/

const int TSPIKEWAVE_LEN = 32; 

struct TSpikeWave_t {
  uint8_t srcchan; 
  uint8_t valid; 
  uint32_t filtid; 
  int32_t threshold; 
  int32_t wave[TSPIKEWAVE_LEN]; 
};


struct TSpike_t
{
  uint8_t src; 
  uint64_t time; 
  TSpikeWave_t x; 
  TSpikeWave_t y; 
  TSpikeWave_t a; 
  TSpikeWave_t b; 

};


TSpike_t rawToTSpike(pDataPacket_t rd); 
pDataPacket_t rawFromTSpikeForTX(const TSpike_t & ts, 
				 sequence_t seq, size_t* len); 
pDataPacket_t rawFromTSpike(const TSpike_t & ts); 


#endif // TSPIKE_TYPE_H

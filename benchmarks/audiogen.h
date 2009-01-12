#ifndef AUDIOGEN_H
#define AUDIOGEN_H

#include <datapacket.h>
#include <tspike.h>
#include <event.h>
#include <wave.h>
#include "eventsource.h"

using namespace somanetwork; 

class AudioGenerator : public EventSource
{
public:
  AudioGenerator(int chan, float tonefreq); 
  void addEvents(somatime_t time, EventList_t * elt); 
  
  static const int FS = 8000; 
  static const int PKTPERBUF = 4; 

  
protected:

  int16_t generateSample(float t); 

  int chan_; 
  float tonefreq_; 
  somatime_t lastts_; 
  float timeSinceSending_; 
  float internalt_; 
  int16_t lastdummyval_; 

}; 


#endif // AUDIOGEN_H


#include <math.h>
#include "audiogen.h"


AudioGenerator::AudioGenerator(int chan, float tonefreq) :
  chan_(chan), 
  tonefreq_(tonefreq), 
  lastts_(0), 
  timeSinceSending_(-1.0), 
  internalt_(0.0), 
  lastdummyval_(0)
{


}

void AudioGenerator::addEvents(somatime_t time, EventList_t * pelt)
{

  // no op right now
  if (timeSinceSending_ == -1 and lastts_ ==0 ) {
    // first pass through, just set / update values
    timeSinceSending_ = 0; 
  } else {
    somatime_t tsdelta = time - lastts_; 
    
    double timedelta = tsdelta * (1.0/50000.0); 
    timeSinceSending_ += timedelta; 

    double packetdur = (1.0 / FS) * PKTPERBUF; 
    while (timeSinceSending_ > packetdur) {
      // send the packet
      Event_t event; 
      event.src = 0x20; 
      event.cmd = 0x30; 
      event.data[0] = chan_; 
      for(int i = 0; i < PKTPERBUF; i++) {
	event.data[i +1] = generateSample(internalt_); 
	internalt_ += (1.0/FS); 
      }
      pelt->push_back(event); 
      timeSinceSending_ -= packetdur; 
    }
    // 
  }

  lastts_ = time; 

}

int16_t AudioGenerator::generateSample(float t)
{
  
  float x = sin(t * tonefreq_ * 3.14 * 2); 
  float yf = x * ((1 << 15) -1); 
  float yr = round(yf); 
  int16_t yi = (int)yr; 
  //yi= lastdummyval_; 
  lastdummyval_++; 

  return yi; 

}

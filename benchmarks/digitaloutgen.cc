#include <math.h>
#include "digitaloutgen.h"


DigitalOutputGenerator::DigitalOutputGenerator(patternList_t pattern, 
					       std::list<int> initvals): 
  channelState_(32)
  
{
  // copy all the patterns:
  std::list<int>::iterator pinit = initvals.begin(); 
  for(patternList_t::iterator p = pattern.begin(); p != pattern.end(); 
      p++) {

    channels_.push_back(p->channel); 
    periods_.push_back(p->period); 
    offsets_.push_back(p->offset);
    dutycycles_.push_back((int) (p->duty*(double)p->period));
    lastsend_.push_back(*pinit);
    
    pinit++; 
  }
  std::vector<int> lastsend_; 
  std::vector<int> channelState_;

}

void DigitalOutputGenerator::addEvents(somatime_t time, EventList_t * pelt)
{
  // I'm pretty sure this is called every ecycle
  bool sendDelta = false;
  somatime_t reltime;

  for(int i = 0; i < channels_.size(); i++) {

    //compute the timestamp relative to beginning of period
    reltime = (time - offsets_[i]) % (periods_[i]);

    if ( (reltime<dutycycles_[i]) && (channelState_[channels_[i]]==0) ) { 
      //we are in the high part of the cycle
      //so the change the state accordingly
      channelState_[channels_[i]] = 1; 
      sendDelta = true;
    } else if ( (reltime>=dutycycles_[i]) && (channelState_[channels_[i]]==1) ) {
      //we are in the low part of the cycle
      //so change the state accordingly
      channelState_[channels_[i]] = 0;
      sendDelta = true;
    }
  }
  if (sendDelta) { 

    Event_t event; 
    event.src = DO_SOURCE;  
    event.cmd = DO_CMD; 
    uint32_t state = 0; 
    for (int i = 31; i >= 0; i--) {
      if(channelState_[i] == 0) {
	state = (state << 1) | 0; 

      } else {
	state = (state << 1) | 1; 
      }
    }      
    event.data[0] = state >> 16; 
    event.data[1] = state & 0xFFFF; 
    
    // now the relevant timestamp
    event.data[2] = time >> 32;
    event.data[3] = (time >> 16);
    event.data[4] = (time) & 0xFFFF;
    pelt->push_back(event); 
    
  }
}


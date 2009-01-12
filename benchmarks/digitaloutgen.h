#ifndef DIGITALOUTGEN_H
#define DIGITALOUTGEN_H

#include <datapacket.h>
#include <tspike.h>
#include <event.h>
#include <wave.h>
#include "eventsource.h"
#include "periodicpattern.h"

/*
    Generates the periodic digital output events. We assume
    that we take in a PeriodicPattern instance which holds
    the channel number, the cycle period in ecycles,
    the phase offset in ecycles and the duty cycle
    as a fraction of the period.
*/


using namespace somanetwork; 

class DigitalOutputGenerator : public EventSource
{
public:
  typedef std::list<PeriodicPattern> patternList_t; 
  DigitalOutputGenerator(patternList_t pattern, std::list<int> initvals); 
  void addEvents(somatime_t time, EventList_t * elt); 
  
protected:

  std::vector<int> channels_; 
  std::vector<int> periods_;
  std::vector<int> offsets_;
  std::vector<int> dutycycles_;
  std::vector<int> lastsend_; 
  std::vector<int> channelState_; 

  const  static int DO_SOURCE = 0x4A;
  const static int DO_CMD = 0x30; 
}; 


#endif // AUDIOGEN_H


#ifndef EVENTSOURCE_H
#define EVENTSOURCE_H

#include <somanetwork/event.h>
#include <somanetwork/datapacket.h>
#include <somanetwork/tspike.h>
#include <somanetwork/wave.h>
#include <somanetwork/ports.h>
#include "eventsynthtx.h"

using namespace somanetwork; 

class EventSource
{
public:
  virtual void addEvents(somatime_t, EventList_t * elt) = 0; 


}; 


#endif // EVENTSOURCE_H

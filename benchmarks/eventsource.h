#ifndef EVENTSOURCE_H
#define EVENTSOURCE_H

#include <event.h>
#include <datapacket.h>
#include <tspike.h>
#include <wave.h>
#include <ports.h>
#include "eventsynthtx.h"

using namespace somanetwork; 

class EventSource
{
public:
  virtual void addEvents(somatime_t, EventList_t * elt) = 0; 


}; 


#endif // EVENTSOURCE_H

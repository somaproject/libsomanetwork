#ifndef EVENTSYNTHTX_H
#define EVENTSYNTHTX_H

#include <iostream>
#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"  
#include "boost/algorithm/string/split.hpp" 
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <datapacket.h>
#include <tspike.h>
#include <event.h>
#include <wave.h>
#include <ports.h>
#include <limits>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include "range.h"

using namespace boost::filesystem; 
using namespace std; 
namespace po = boost::program_options;


typedef int64_t somatime_t; 

class EventSource; 

struct eventtxparams_t
{
  double elapsed; 
  sequence_t seq; 
  int ecycleload; 
  sockaddr_in si_addr; 
  int count; 
  int socket; 
  somatime_t latesttime; 
  std::list<EventSource *> eventSources_; 

}; 

uint64_t usecdelta(timeval & t1, timeval & t2); 

int  event_send_packet(eventtxparams_t *  params); 

void event_setup_socket(eventtxparams_t *  params, std::string destipaddr); 

void eventtx_thread(eventtxparams_t *  params); 

#endif // EVENTSYNTTX_H

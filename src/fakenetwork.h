#ifndef FAKENETWORK_H
#define FAKENETWORK_H

#include <map>
#include <utility>
#include <sys/epoll.h>
#include <fstream>

#include "tspipefifo.h"
#include "datareceiver.h"
#include "eventreceiver.h"
#include "event.h"
#include "networkinterface.h"
#include <sigc++/sigc++.h>
/* 
There's the tricky problem of how to handine inbound events in the 
fake network interface, which will be appended as a EventTXList_t  
via sendEvents. We elect to use a sigc++ callback such that 
the resulting event processing (and manipulation of the network)
will take place in the calling thread; and then potentially block 
on the response. 


*/


typedef std::pair<datasource_t, datatype_t> datagen_t; 

typedef sigc::signal<void, const EventTXList_t &> signalEventTX_t; 


class FakeNetwork : public NetworkInterface
{

 public:
  FakeNetwork(); 
  ~FakeNetwork(); 
  
  void enableDataRX(datasource_t, datatype_t); 
  void disableDataRX(datasource_t, datatype_t); 
  
  pDataPacket_t  getNewData(void); 
  pEventList_t getNewEvents(void); 

  int getDataFifoPipe(); 
  int getEventFifoPipe(); 
  void run(); 
  void shutdown(); 

  void appendDataOut(pDataPacket_t out); 
  void appendEventOut(pEventList_t out); 
  
  eventtxnonce_t sendEvents(const EventTXList_t & el);
  signalEventTX_t & signalEventTX();  // returns ref to the relevant signal

  std::vector<DataReceiverStats>  getDataStats(); 
  
 private: 
  TSPipeFifo<pDataPacket_t> outputDataFifo_; 
  TSPipeFifo<pEventList_t> outputEventFifo_; 

  bool running_; 

  boost::thread *  pthrd_; 
  boost::mutex appendMutex_; 
  void workthread(void); 
  
  signalEventTX_t signalEventTX_; 

};

#endif // FAKENETWORK_H


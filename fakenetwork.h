#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <utility>
#include <sys/epoll.h>


#include "tspipefifo.h"
#include "datareceiver.h"
#include "eventreceiver.h"
#include "data/event.h"
#include "networkinterface.h"


typedef std::pair<datasource_t, datatype_t> datagen_t; 

class FakeNetwork : public NetworkInterface
{

 public:
  FakeNetwork(); 
  ~FakeNetwork(); 
  
  void enableDataRX(datasource_t, datatype_t); 
  void disableDataRX(datasource_t, datatype_t); 
  
  DataPacket_t*  getNewData(void); 
  EventList_t * getNewEvents(void); 

  int getDataFifoPipe(); 
  int getEventFifoPipe(); 
  void run(); 
  void shutdown(); 

  void appendDataOut(DataPacket_t * out); 
  void appendEventOut(EventList_t * out); 
  
  std::vector<DataReceiverStats>  getDataStats(); 
  
 private: 
  TSPipeFifo<DataPacket_t*> outputDataFifo_; 
  TSPipeFifo<EventList_t*> outputEventFifo_; 

  bool running_; 

  boost::thread *  pthrd_; 
  void workthread(void); 

};

#endif // NETWORK_H


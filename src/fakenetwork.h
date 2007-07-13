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
  
  eventtxnonce_t sendEvents(const EventTXList_t & el);

  std::vector<DataReceiverStats>  getDataStats(); 
  
 private: 
  TSPipeFifo<DataPacket_t*> outputDataFifo_; 
  TSPipeFifo<EventList_t*> outputEventFifo_; 

  bool running_; 

  boost::thread *  pthrd_; 
  boost::mutex appendMutex_; 
  void workthread(void); 
  

};

#endif // FAKENETWORK_H


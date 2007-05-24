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

class Network : public NetworkInterface

{

 public:
  Network(); 
  ~Network(); 
  
  void enableDataRX(datasource_t, datatype_t); 
  void disableDataRX(datasource_t, datatype_t); 
  
  DataPacket_t*  getNewData(void); 
  EventList_t * getNewEvents(void); 

  int getDataFifoPipe(); 
  int getEventFifoPipe(); 
  void run(); 
  void shutdown(); 
  
  std::vector<DataReceiverStats>  getDataStats(); 
  
 private: 
  TSPipeFifo<DataPacket_t*> outputDataFifo_; 
  TSPipeFifo<EventList_t*> outputEventFifo_; 

  std::map<const datagen_t, DataReceiver*> dataReceivers_; 
  EventReceiver eventReceiver_; 
  void appendDataOut(DataPacket_t* out); 
  void appendEventOut(EventList_t * out); 
  bool running_; 
  
  int epollfd_; 
  boost::thread *  pthrd_; 
  void workthread(void); 

};

#endif // NETWORK_H


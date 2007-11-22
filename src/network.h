#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <utility>
#include <sys/epoll.h>
#include "tspipefifo.h"
#include "datareceiver.h"
#include "eventreceiver.h"
#include "eventsender.h"
#include "event.h"
#include "networkinterface.h"

typedef std::pair<datasource_t, datatype_t> datagen_t; 

class Network : public NetworkInterface

{

 public:
  Network(std::string SomaIP); 
  ~Network(); 
  
  void enableDataRX(datasource_t, datatype_t); 
  void disableDataRX(datasource_t, datatype_t); 
  
  pDataPacket_t  getNewData(void); 
  pEventList_t getNewEvents(void); 

  eventtxnonce_t sendEvents(const EventTXList_t & el);


  int getDataFifoPipe(); 
  int getEventFifoPipe(); 
  void run(); 
  void shutdown(); 
  
  std::vector<DataReceiverStats>  getDataStats(); 
  EventReceiverStats getEventStats(); 

 private: 
  TSPipeFifo<pDataPacket_t> outputDataFifo_; 
  TSPipeFifo<pEventList_t> outputEventFifo_; 

  eventDispatcherPtr_t pDispatch_; 

  std::map<const datagen_t, DataReceiver*> dataReceivers_; 
  EventReceiver eventReceiver_; 
  EventSender eventSender_; 

  void appendDataOut(pDataPacket_t out); 
  void appendEventOut(pEventList_t out); 
  bool running_; 
  
  boost::thread *  pthrd_; 
  void workthread(void); 

  

};

#endif // NETWORK_H


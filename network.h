#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <utility>


#include "tspipefifo.h"
#include "datareceiver.h"

typedef unsigned char datasource_t; 
typedef unsigned char datatype_t; 

typedef std::pair<datasource_t, datatype_t> datagen_t; 

class Network
{

 public:
  Network(); 
  ~Network(); 
  
  void enableDataRx(datasource_t, datatype_t); 
  void disableDataRx(datasource_t, datatype_t); 

  RawData*  getNewData(void); 
  int getTSPipeFifoPipe(); 
  void run(); 
  void shutdown(); 

 private: 
  TSPipeFifo<RawData*> outputFifo_; 
  std::map<const datagen_t, DataReceiver*> dataReceivers_; 
  void appendDataOut(RawData* out); 
  asio::io_service ioservice_; 

  boost::thread *  pthrd_; 
  void workthread(void); 

};

#endif // NETWORK_H


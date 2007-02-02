#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <utility>
#include <sys/epoll.h>


#include "tspipefifo.h"
#include "datareceiver.h"


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
  
  std::vector<DataReceiverStats>  getDataStats(); 

 private: 
  TSPipeFifo<RawData*> outputFifo_; 
  std::map<const datagen_t, DataReceiver*> dataReceivers_; 
  void appendDataOut(RawData* out); 

  bool shuttingDown_; 

  int epollfd_; 
  boost::thread *  pthrd_; 
  void workthread(void); 

};

#endif // NETWORK_H


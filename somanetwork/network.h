#ifndef SOMANETWORK_NETWORK_H
#define SOMANETWORK_NETWORK_H

#include <map>
#include <utility>
#include <sys/epoll.h>
#include <boost/filesystem.hpp>
#include <somanetwork/sockproxy.h>
#include <somanetwork/tspipefifo.h> 
#include <somanetwork/datareceiver.h> 
#include <somanetwork/eventreceiver.h>
#include <somanetwork/eventsender.h> 
#include <somanetwork/event.h> 
#include <somanetwork/networkinterface.h>

namespace somanetwork {

class Network : public NetworkInterface

{

 public:
  static pNetworkInterface_t createINet(std::string somaip); 
  static pNetworkInterface_t createDomain(boost::filesystem::path basedirectory); 

  ~Network(); 

  
  void enableDataRX(datasource_t, datatype_t); 
  void disableDataRX(datasource_t, datatype_t); 
  void disableAllDataRX(); 

  void enableEventRX(); 
  void disableEventRX(); 


  pDataPacket_t  getNewData(void); 
  pEventPacket_t getNewEvents(void); 

  eventtxnonce_t sendEvents(const EventTXList_t & el);


  int getDataFifoPipe(); 
  int getEventFifoPipe(); 
  void run(); 
  void shutdown(); 
  
  std::vector<DataReceiverStats>  getDataStats(); 
  SeqPacketProtoStats getEventStats(); 

  void resetDataStats(); 
  void resetEventStats(); 


 private: 
  Network(pISocketProxy_t sockproxy); 

  pISocketProxy_t pSockProxy_; 
  TSPipeFifo<pDataPacket_t> outputDataFifo_; 
  TSPipeFifo<pEventPacket_t> outputEventFifo_; 

  eventDispatcherPtr_t pDispatch_; 

  std::map<const datagen_t, DataReceiver*> dataReceivers_; 
  EventReceiver eventReceiver_; 
  EventSender eventSender_; 

  void appendDataOut(pDataPacket_t out); 
  void appendEventOut(pEventPacket_t out); 
  bool running_; 
  
  boost::thread *  pthrd_; 
  void workthread(void); 

  bool eventRXEnabled_; 


};
  
}
#endif // NETWORK_H


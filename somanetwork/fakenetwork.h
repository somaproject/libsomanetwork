#ifndef SOMANETWORK_FAKENETWORK_H
#define SOMANETWORK_FAKENETWORK_H

#include <map>
#include <set>
#include <utility>
//#include <sys/epoll.h>
#include <fstream>

#include <somanetwork/tspipefifo.h>
#include <somanetwork/datareceiver.h>
#include <somanetwork/eventreceiver.h>
#include <somanetwork/soma_event.h>
#include <somanetwork/networkinterface.h>
#include <sigc++/sigc++.h>
/* 
There's the tricky problem of how to handine inbound events in the 
fake network interface, which will be appended as a EventTXList_t  
via sendEvents. We elect to use a sigc++ callback such that 
the resulting event processing (and manipulation of the network)
will take place in the calling thread; and then potentially block 
on the response. 


*/

namespace somanetwork { 
typedef sigc::signal<void, const EventTXList_t &> signalEventTX_t; 


class FakeNetwork : public NetworkInterface
{

 public:
  FakeNetwork(); 
  ~FakeNetwork(); 
  
  void enableDataRX(datasource_t, datatype_t); 
  void disableDataRX(datasource_t, datatype_t); 
  void disableAllDataRX(); 
  
  pDataPacket_t  getNewData(void); 
  pEventPacket_t getNewEvents(void); 

  int getDataFifoPipe(); 
  int getEventFifoPipe(); 
  void run(); 
  void shutdown(); 

  void appendDataOut(pDataPacket_t out); 
  void appendEventOut(pEventPacket_t out); 
  
  eventtxnonce_t sendEvents(const EventTXList_t & el);
  signalEventTX_t & signalEventTX();  // returns ref to the relevant signal

  void setDataStats(std::vector<DataReceiverStats> ds); 
  std::vector<DataReceiverStats> getDataStats(); 
  void resetDataStats(); 

  SeqPacketProtoStats getEventStats(); 

  void resetEventStats(); 

 private: 
  TSPipeFifo<pDataPacket_t> outputDataFifo_; 
  TSPipeFifo<pEventPacket_t> outputEventFifo_; 

  bool running_; 

  boost::thread * pthrd_; 
  boost::mutex appendMutex_; 
  void workthread(void); 
  
  signalEventTX_t signalEventTX_; 
  std::set<datagen_t> dataReceivers_; 
  
  std::vector<DataReceiverStats>  currentStats_; 

};

typedef boost::shared_ptr<FakeNetwork> pFakeNetwork_t; 
}
#endif // FAKENETWORK_H


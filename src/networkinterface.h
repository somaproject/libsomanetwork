#ifndef NETWORKINTERFACE_H 
#define NETWORKINTERFACE_H

#include <boost/shared_ptr.hpp>

#include "event.h"
#include "datapacket.h"
#include "eventtx.h"
#include "datareceiver.h"
#include "eventreceiver.h" 

namespace somanetwork { 

typedef std::pair<datasource_t, datatype_t> datagen_t; 

class NetworkInterface
{

 public: 
/*   virtual Network() = 0;  */
/*   virtual ~Network() = 0;  */
  
  virtual void enableDataRX(datasource_t, datatype_t) = 0;
  virtual void disableDataRX(datasource_t, datatype_t) = 0; 
  virtual void disableAllDataRX() = 0; 
  
  virtual pDataPacket_t  getNewData(void) = 0; 
  virtual pEventPacket_t  getNewEvents(void) = 0; 

  virtual int getDataFifoPipe() = 0; 
  virtual int getEventFifoPipe() = 0;
  virtual void run() = 0; 
  virtual void shutdown() = 0; 
  
  virtual std::vector<DataReceiverStats>  getDataStats() = 0; 
  virtual void resetDataStats() = 0; 
  

  virtual eventtxnonce_t sendEvents(const EventTXList_t & el) = 0; 

  virtual EventReceiverStats getEventStats() = 0; 
  virtual void resetEventStats() = 0; 

  //virtual ~NetworkInterface() =0; 
}; 

typedef boost::shared_ptr<NetworkInterface> pNetworkInterface_t; 

}
#endif // NETWORKINTERFACE_H

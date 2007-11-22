#ifndef NETWORKINTERFACE_H 
#define NETWORKINTERFACE_H

#include "event.h"
#include "datapacket.h"
#include "eventtx.h"
#include "datareceiver.h"

class NetworkInterface
{

 public: 
/*   virtual Network() = 0;  */
/*   virtual ~Network() = 0;  */
  
  virtual void enableDataRX(datasource_t, datatype_t) = 0;
  virtual void disableDataRX(datasource_t, datatype_t) = 0; 
  
  virtual pDataPacket_t  getNewData(void) = 0; 
  virtual EventList_t * getNewEvents(void) = 0; 

  virtual int getDataFifoPipe() = 0; 
  virtual int getEventFifoPipe() = 0;
  virtual void run() = 0; 
  virtual void shutdown() = 0; 
  
  virtual std::vector<DataReceiverStats>  getDataStats() = 0; 

  virtual eventtxnonce_t sendEvents(const EventTXList_t & el) = 0; 

}; 

#endif // NETWORKINTERFACE_H

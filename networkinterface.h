#ifndef NETWORKINTERFACE_H 
#define NETWORKINTERFACE_H

#include "data/event.h"
#include "data/rawdata.h"
#include "datareceiver.h"

class NetworkInterface
{

 public: 
/*   virtual Network() = 0;  */
/*   virtual ~Network() = 0;  */
  
  virtual void enableDataRX(datasource_t, datatype_t) = 0;
  virtual void disableDataRX(datasource_t, datatype_t) = 0; 
  
  virtual DataPacket_t*  getNewData(void) = 0; 
  virtual EventList_t * getNewEvents(void) = 0; 

  virtual int getDataFifoPipe() = 0; 
  virtual int getEventFifoPipe() = 0;
  virtual void run() = 0; 
  virtual void shutdown() = 0; 
  
  virtual std::vector<DataReceiverStats>  getDataStats() = 0; 

}; 

#endif // NETWORKINTERFACE_H

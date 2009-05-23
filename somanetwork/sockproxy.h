#ifndef SOMANETWORK_SOCKPROXY_H
#define SOMANETWORK_SOCKPROXY_H

#include <boost/shared_ptr.hpp>
#include <arpa/inet.h>

#include <somanetwork/ports.h>
#include <somanetwork/datapacket.h>


namespace somanetwork { 

class ISocketProxy
{

public:

  // open and return a data socket suitable for receiving data
  virtual int createDataSocket(datasource_t src, datatype_t type) = 0; 
  
  virtual sockaddr * getDataReTxReqSockAddr() = 0;
  virtual socklen_t getDataReTxReqSockAddrLen() =0; 

  // Event interactions
  virtual int createEventRXSocket() = 0; 
  virtual sockaddr * getEventReTxReqSockAddr() = 0; 
  virtual socklen_t getEventReTxReqSockAddrLen() = 0; 
  
  virtual int createEventTXSocket() = 0; 
  virtual sockaddr * getEventTXSockAddr() = 0; 
  virtual socklen_t getEventTXSockAddrLen() = 0; 

}; 

  typedef boost::shared_ptr<ISocketProxy> pISocketProxy_t; 

}
#endif // SOMANETWORK_SOCKPROXY_H

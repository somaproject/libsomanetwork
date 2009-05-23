#ifndef SOMANETWORK_NETSOCKPROXY_H
#define SOMANETWORK_NETSOCKPROXY_H

#include <string>
#include <somanetwork/sockproxy.h>

namespace somanetwork {

class NetSocketProxy : public ISocketProxy
{
public: 

  NetSocketProxy(std::string somaip); 

  // open and return a data socket suitable for receiving data
  int createDataSocket(datasource_t src, datatype_t type); 
  
  sockaddr * getDataReTxReqSockAddr() {
    return (sockaddr*)&retxDataReqSockAddr_; 
  }

  inline socklen_t getDataReTxReqSockAddrLen() {
    return sizeof(sockaddr_in); 
  }

  // Event interactions
  int createEventRXSocket(); 

  inline sockaddr * getEventReTxReqSockAddr() {
    return (sockaddr*)&retxEventReqSockAddr_; 
  }
  
  inline socklen_t getEventReTxReqSockAddrLen() {
    return sizeof(sockaddr_in); 
  }
  
  int createEventTXSocket(); 

  inline sockaddr * getEventTXSockAddr() {
    return (sockaddr*)&eventTXSockAddr_; 
  }

  inline socklen_t getEventTXSockAddrLen() {
    return sizeof(sockaddr_in); 
  }

private:
  std::string somaip_; 
  sockaddr_in retxDataReqSockAddr_; 
  sockaddr_in retxEventReqSockAddr_; 
  sockaddr_in eventTXSockAddr_; 

}; 

  




}


#endif // SOMANETWORK_SOCKPROXY_H

#ifndef SOMANETWORK_DOMAINSOCKPROXY_H
#define SOMANETWORK_DOMAINSOCKPROXY_H

#include <string>
#include <boost/filesystem.hpp>  
#include <somanetwork/sockproxy.h>
#include <sys/socket.h>
#include <sys/un.h>


namespace somanetwork {

class DomainSocketProxy : public ISocketProxy
{
public: 

  DomainSocketProxy(boost::filesystem::path rootdir); 

  // open and return a data socket suitable for receiving data
  int createDataSocket(datasource_t src, datatype_t type); 
  
  sockaddr * getDataReTxReqSockAddr() {
    return (sockaddr*)&retxDataReqSockAddr_; 
  }

  inline socklen_t getDataReTxReqSockAddrLen() {
    return sizeof(sockaddr_un); 
  }

  // Event interactions
  int createEventRXSocket(); 

  inline sockaddr * getEventReTxReqSockAddr() {
    return (sockaddr*)&retxEventReqSockAddr_; 
  }
  
  inline socklen_t getEventReTxReqSockAddrLen() {
    return sizeof(sockaddr_un); 
  }
  
  int createEventTXSocket(); 

  inline sockaddr * getEventTXSockAddr() {
    return (sockaddr*)&eventTXSockAddr_; 
  }

  inline socklen_t getEventTXSockAddrLen() {
    return eventTXSockAddrLen_; 
  }

private:
  boost::filesystem::path rootdir_; 
  sockaddr_un retxDataReqSockAddr_; 
  sockaddr_un retxEventReqSockAddr_; 

  sockaddr_un eventTXSockAddr_; 
  socklen_t eventTXSockAddrLen_; 
}; 


}

#endif

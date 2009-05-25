#include "netsockproxy.h"


namespace somanetwork {
  
  NetSocketProxy::NetSocketProxy(std::string somaip) :
    somaip_(somaip)
  {
    
    /* initially create the sockaddr structures */ 
    retxDataReqSockAddr_.sin_port = htons(DATARETXPORT); 
    retxDataReqSockAddr_.sin_family = AF_INET;
    inet_pton(AF_INET, somaip_.c_str(), &retxDataReqSockAddr_.sin_addr);
    
    retxEventReqSockAddr_.sin_port = htons(EVENTRXRETXPORT); 
    retxEventReqSockAddr_.sin_family = AF_INET;
    inet_pton(AF_INET, somaip_.c_str(), &retxEventReqSockAddr_.sin_addr);

    eventTXSockAddr_.sin_port = htons(EVENTTXPORT); 
    eventTXSockAddr_.sin_family = AF_INET;
    inet_pton(AF_INET, somaip_.c_str(), &eventTXSockAddr_.sin_addr);
     
  }

  int NetSocketProxy::createDataSocket(datasource_t src, datatype_t type) {

    int sock; 
    
    struct sockaddr_in si_me, si_other;
    int  slen=sizeof(si_other);
    
    sock = socket(AF_INET, SOCK_DGRAM, 17); 
    if (sock < 0) {
      throw std::runtime_error("could not create socket"); 
      
    }
    
    bzero((char *) &si_me, sizeof(si_me));
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(dataPortLookup(type,  src));
    
    si_me.sin_addr.s_addr = INADDR_ANY; 
    
    int optval = 1; 
    
    // configure sock for reuse
    optval = 1; 
    int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
			 &optval, sizeof (optval)); 
    if (res < 0) {
      throw std::runtime_error("error setting socket to reuse"); 
    }
    
    optval = 1; 
    res = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, 
		     &optval, sizeof (optval)); 
    if (res < 0) {
      throw std::runtime_error("error setting the broadcast bit"); 
    }
    
    optval = 500000; 
    res = setsockopt (sock, SOL_SOCKET, SO_RCVBUF, 
		      (const void *) &optval, sizeof(optval)); 
    if (res < 0) {
      throw std::runtime_error("error settng receive buffer size"); 
      
    }
    
    res =  bind(sock, (sockaddr*)&si_me, sizeof(si_me)); 
    if (res < 0) {
      throw std::runtime_error("error binding socket"); 
    }
    
    return sock; 

  }


  int NetSocketProxy::createEventRXSocket() {
    int sock; 
    struct sockaddr_in si_me, si_other;
    int  slen=sizeof(si_other);
    
    sock = socket(AF_INET, SOCK_DGRAM, 17); 
    if (sock < 0) {
      throw std::runtime_error("could not create socket"); 
      
    }
    
    bzero((char *) &si_me, sizeof(si_me));
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(EVENTRXPORT); 
    
    si_me.sin_addr.s_addr = INADDR_ANY; 
    
    int optval = 1; 
    
    // confiugre socket for reuse
    optval = 1; 
    int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, 
			 &optval, sizeof (optval)); 
    if (res < 0) {
      throw std::runtime_error("error settng socket to reuse"); 
    }
    
    optval = 4000000; 
    res = setsockopt (sock, SOL_SOCKET, SO_RCVBUF, 
		      (const void *) &optval, sizeof(optval)); 
    if (res < 0) {
      throw std::runtime_error("error settng receive buffer size"); 
    }
    
    socklen_t optlen = 0;   
    res = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, 
		     (void *) &optval, &optlen); 
    
    res =  bind(sock, (sockaddr*)&si_me, sizeof(si_me)); 
    if (res < 0) {
      throw std::runtime_error("error binding socket"); 
    }
    
    return sock;
    
  }


  int NetSocketProxy::createEventTXSocket() {

    int sock = socket(AF_INET, SOCK_DGRAM, 17); 
    if (sock  < 0) {
      throw std::runtime_error("could not create transmit socket"); 
    }
    
    return sock; 
    
  }

  


}

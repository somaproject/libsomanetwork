#include <boost/format.hpp>
#include <iostream>

#include "domainsockproxy.h"

namespace somanetwork { 
  namespace bf = boost::filesystem; 
  
  DomainSocketProxy::DomainSocketProxy(bf::path rootdir)  : 
    rootdir_(rootdir) 
  {

    if(! bf::is_directory(rootdir_) ) {
      throw std::runtime_error("DomainSocketProxy was not given a valid root directory"); 
      
    }

    // Check if the following files exist -- not quite the same as checking
    // if they're domain sockets, but a good sanity check
    if ( !bf::exists(rootdir_ / "dataretx")) {
      throw std::runtime_error("DomainSocketProxy dataretx domain socket does not exist"); 
    }
    
    if ( !bf::exists(rootdir_ / "eventretx")) {
      throw std::runtime_error("DomainSocketProxy eventretx domain socket does not exist"); 
    }
    
    if ( !bf::exists(rootdir_ / "eventtx")) {
      throw std::runtime_error("DomainSocketProxy eventtx domain socket does not exist"); 
    }
    
    // unlink all possible data sockets
    bf::remove_all(rootdir_ / " data"); 
    
    // now create directory
    bf::create_directories(rootdir_ / "data"); 

    bf::create_directories(rootdir_ / "data" / "tspike"); 
    bf::create_directories(rootdir_ / "data" / "raw"); 
    bf::create_directories(rootdir_ / "data" / "wave"); 
    
    // set up the EventTX Sock Addr structure
    eventTXSockAddr_.sun_family = AF_LOCAL; 
    std::string eventtxpath = (rootdir_ / "eventtx").string(); 

    strcpy(eventTXSockAddr_.sun_path, eventtxpath.c_str()); 

    eventTXSockAddrLen_ = sizeof(eventTXSockAddr_.sun_family) + 
      strlen(eventTXSockAddr_.sun_path); 
    

  }

  int DomainSocketProxy::createDataSocket(datasource_t src, datatype_t type)
  {

    int sock; 
    
    sock = socket(AF_LOCAL, SOCK_DGRAM, 0); 
    if (sock < 0) {
      throw std::runtime_error("could not create socket"); 
      
    }
    
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

    struct sockaddr_un su_me; 
    

    bzero((char *) &su_me, sizeof(su_me));
    
    su_me.sun_family = AF_LOCAL;
    bf::path srcpath(rootdir_ / "data"); 
    bf::path destpath; 
    if (type == TSPIKE) {
      destpath = srcpath /  bf::path("tspike") / boost::str(boost::format("%d") % (int)src); 
    } else if (type == RAW) {
      destpath = srcpath /  bf::path("raw") / boost::str(boost::format("%d") % (int)src); 
    } else if (type == WAVE) {
      destpath = srcpath /  bf::path("wave") / boost::str(boost::format("%d") % (int)src); 
    } else {
      throw std::runtime_error("unimplemented data type"); 
    }

    strcpy(su_me.sun_path, destpath.string().c_str()); 
    size_t len = sizeof(su_me.sun_family) + strlen(su_me.sun_path); 
    res =  bind(sock, (sockaddr*)&su_me, len); 
    if (res < 0) {
      std::cout << su_me.sun_path << std::endl; 
      int errv =errno; // save errno
      boost::format errormsg("Domain socket proxy: error binding data socket, err '%s'"); 
      throw std::runtime_error(boost::str(errormsg % strerror(errv))); 

    }
    
    return sock; 

    

  }

  int DomainSocketProxy::createEventRXSocket() {
    
    int sock; 
    
    sock = socket(AF_LOCAL, SOCK_DGRAM, 0); 
    if (sock < 0) {
      throw std::runtime_error("Could not create socket"); 
      
    }

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


    struct sockaddr_un su_me, su_other;
    int  slen=sizeof(su_other);
    
    bzero((char *) &su_me, sizeof(su_me));
    
    su_me.sun_family = AF_LOCAL;
    bf::path srcpath(rootdir_ / "eventrx"); 

    strcpy(su_me.sun_path, srcpath.string().c_str()); 
    
    res =  bind(sock, (sockaddr*)&su_me, sizeof(su_me)); 
    if (res < 0) {
      throw std::runtime_error("error binding socket"); 
    }
    
    return sock;
    


  }

  int DomainSocketProxy::createEventTXSocket() {
    bf::path  sendpath = rootdir_ / "eventtx_sender"; 
    int sock = socket(AF_LOCAL, SOCK_DGRAM, 0); 
    // we must explicitly bind a name
    sockaddr_un sa; 
    sa.sun_family =AF_LOCAL; 
    strcpy(sa.sun_path, sendpath.string().c_str()); 
    bind(sock, (sockaddr*)&sa, SUN_LEN(&sa)); 

    return sock; 

  } 


}

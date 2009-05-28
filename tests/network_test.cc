#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#include <somanetwork/datareceiver.h>
#include <somanetwork/network.h>

#include <sys/un.h>
#include <sys/socket.h>

#include "tests.h"


using namespace boost::filesystem; 
using namespace std; 
using namespace somanetwork; 
namespace bf = boost::filesystem; 

BOOST_AUTO_TEST_SUITE(network_test); 


bf::path createTempDir()
{
  char tempdir[] = "/tmp/testXXXXXX"; 
  char * ptr = mkdtemp(tempdir); 
  if(ptr == NULL) {
    throw std::runtime_error("unable to create temporary directory"); 
  }

  return bf::path(ptr); 
}

int createBoundDomainSocket(bf::path srcpath)
{
  // Create and bind a temporary domain socket at the indicated path

  int sock = socket(AF_LOCAL, SOCK_DGRAM, 0); 
  
  if (sock < 0) {
    throw std::runtime_error("test infrastructure could not create socket"); 
    
  }
  
  struct sockaddr_un su_me; 
  
  
  bzero((char *) &su_me, sizeof(su_me));
  
  su_me.sun_family = AF_LOCAL;
  
  strcpy(su_me.sun_path, srcpath.string().c_str()); 
  
  int res =  bind(sock, (sockaddr*)&su_me, sizeof(su_me)); 
  
  if (res < 0) {
    throw std::runtime_error("test infrastrucutre error binding socket"); 
  }
  
  return sock; 
  
}

BOOST_AUTO_TEST_CASE(simple)
{
  // Create and destroy network
  pNetworkInterface_t ni = Network::createINet("127.0.0.1"); 
  
  
}

BOOST_AUTO_TEST_CASE(simple_start)
{
  // Create and destroy network
  pNetworkInterface_t ni = Network::createINet("127.0.0.1"); 
  ni->run(); 
  ni->shutdown(); 
  
  
}


BOOST_AUTO_TEST_CASE(simple_start_eanble)
{
  // Create and destroy network
  pNetworkInterface_t ni = Network::createINet("127.0.0.1"); 
  ni->enableDataRX(0, TSPIKE); 
  ni->run(); 
  ni->shutdown(); 
  
  
}


// BOOST_AUTO_TEST_CASE(simple_start_domain)
// {
//   bf::path tempdir = createTempDir(); 

//   createBoundDomainSocket(tempdir / "dataretx"); 
//   createBoundDomainSocket(tempdir / "eventretx"); 
//   createBoundDomainSocket(tempdir / "eventtx"); 
  

//   // Create and destroy network
//   pNetworkInterface_t ni = Network::createDomain(tempdir);
//   ni->enableDataRX(0, TSPIKE); 
//   ni->run(); 
//   sleep(1); 
//   ni->shutdown(); 
  
  
// }


BOOST_AUTO_TEST_SUITE_END(); 

#include <boost/test/auto_unit_test.hpp>
#include <boost/format.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>             
#include <assert.h>       
#include <somanetwork/network.h>
#include <somanetwork/raw.h>
#include <somanetwork/tspike.h>
#include <somanetwork/wave.h>
#include <stdlib.h>
#include <sys/un.h>

#include <sys/socket.h>
#include "canonical.h"


using namespace boost::filesystem; 
namespace bf = boost::filesystem; 
using namespace somanetwork; 

BOOST_AUTO_TEST_SUITE(domainsockproxy_test); 

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


pDataPacket_t sendData(Raw_t raw, bf::path outdir, 
		       sequence_t seq, int sendingsocket) {

  
  size_t  len = 0;

  pDataPacket_t dp = (rawFromRawForTX(raw, seq, &len)); 

  // now construct the packet to transmit
  std::vector<char> buffer(1000); 
  sequence_t netseq = htonl(dp->seq); 
  
  memcpy(&buffer[0], &netseq, sizeof(sequence_t)); 
  // now copy the actual data
  memcpy(&buffer[4], &(dp->body), len); 

  // now send
  sockaddr_un sun; 
  sun.sun_family = AF_LOCAL; 
  bf::path destpath = outdir / "data" 
    / "raw" / boost::str((boost::format("%d") % (int)raw.src)); 

  if(!bf::exists(destpath)) {
    std::cout << destpath << std::endl; 
    throw std::runtime_error("target socket path does not exist"); 
  }
  strcpy(sun.sun_path, destpath.string().c_str()); 

  sendto(sendingsocket, &buffer[0], len+6, 0, (sockaddr*)&sun,
	 SUN_LEN(&sun)); 
	 //sizeof(sun.sun_family) + strlen(sun.sun_path));
  
  return dp; 

}

pDataPacket_t sendData(TSpike_t raw, bf::path outdir, 
		       sequence_t seq, int sendingsocket) {

  
  size_t  len = 0;

  pDataPacket_t dp = (rawFromTSpikeForTX(raw, seq, &len)); 

  // now construct the packet to transmit
  std::vector<char> buffer(1000); 
  sequence_t netseq = htonl(dp->seq); 
  
  memcpy(&buffer[0], &netseq, sizeof(sequence_t)); 
  // now copy the actual data
  memcpy(&buffer[4], &(dp->body), len); 

  // now send
  sockaddr_un sun; 
  sun.sun_family = AF_LOCAL; 
  bf::path destpath = outdir / "data" 
    / "tspike" / boost::str((boost::format("%d") % (int)raw.src)); 

  if(!bf::exists(destpath)) {
    std::cout << destpath << std::endl; 
    throw std::runtime_error("target socket path does not exist"); 
  }
  strcpy(sun.sun_path, destpath.string().c_str()); 

  sendto(sendingsocket, &buffer[0], len+6, 0, (sockaddr*)&sun,
	 sizeof(sun.sun_family) + strlen(sun.sun_path));
  
  return dp; 

}


pDataPacket_t sendData(Wave_t raw, bf::path outdir, 
		       sequence_t seq, int sendingsocket) {

  
  size_t  len = sizeof(Wave_t);
  
  pDataPacket_t dp = rawFromWave(raw); 

  // now construct the packet to transmit
  std::vector<char> buffer(1000); 
  sequence_t netseq = htonl(dp->seq); 
  
  memcpy(&buffer[0], &netseq, sizeof(sequence_t)); 
  // now copy the actual data
  memcpy(&buffer[4], &(dp->body), len); 

  // now send
  sockaddr_un sun; 
  sun.sun_family = AF_LOCAL; 
  bf::path destpath = outdir / "data" 
    / "wave" / boost::str((boost::format("%d") % (int)raw.src)); 

  if(!bf::exists(destpath)) {
    std::cout << destpath << std::endl; 
    throw std::runtime_error("target socket path does not exist"); 
  }
  strcpy(sun.sun_path, destpath.string().c_str()); 

  sendto(sendingsocket, &buffer[0], len+6, 0, (sockaddr*)&sun,
	 sizeof(sun.sun_family) + strlen(sun.sun_path));
  
  return dp; 

}


BOOST_AUTO_TEST_CASE(fail_existing_sockets)
{
  /*
    Test if we correctly fail to construct the DomainSockProxy 
    if the required sockets don't exist
  */ 

  bf::path tempdir = createTempDir(); 
  BOOST_CHECK_THROW(Network::createDomain(tempdir), 
		    std::runtime_error); 
  
  createBoundDomainSocket(tempdir / "dataretx"); 
  createBoundDomainSocket(tempdir / "eventretx"); 
  createBoundDomainSocket(tempdir / "eventtx"); 
  
  pNetworkInterface_t nf = Network::createDomain(tempdir); 
  
}

BOOST_AUTO_TEST_CASE(test_data_from_soma)
{
  /*
    1. Does the domainsockproxy correctly create the sockets for
    inbound data? 
    2. When data is sent to these sockets, does libsomanetwork correctly
    process it? Send a tspike, a wave, and a raw to test. 

  */ 

  bf::path tempdir = createTempDir(); 
  BOOST_CHECK_THROW(Network::createDomain(tempdir), 
		    std::runtime_error); 
  
  createBoundDomainSocket(tempdir / "dataretx"); 
  createBoundDomainSocket(tempdir / "eventretx"); 
  createBoundDomainSocket(tempdir / "eventtx"); 
  
  int sendingsocket = createBoundDomainSocket(tempdir / "test_sendsocket"); 

  pNetworkInterface_t nf = Network::createDomain(tempdir); 
  
  datasource_t src = 10;
  nf->enableDataRX(src, RAW); 
  nf->enableDataRX(src+1, TSPIKE); 
  nf->enableDataRX(src+2, WAVE); 

  nf->run(); 
  
  pDataPacket_t dp_raw = sendData(generateCanonicalRaw(src, 0),
				  tempdir, 100, sendingsocket); 
  
  pDataPacket_t dp_tspike = sendData(generateCanonicalTSpike(src+1, 1000),
				     tempdir, 100, sendingsocket); 
  
  pDataPacket_t dp_wave = sendData(generateCanonicalWave(src+2, 100000),
				   tempdir, 100, sendingsocket); 
  
  char b; 

  read(nf->getDataFifoPipe(), &b, 1); 
  pDataPacket_t newpacket_raw = nf->getNewData(); 


  read(nf->getDataFifoPipe(), &b, 1); 
  pDataPacket_t newpacket_tspike = nf->getNewData(); 

  read(nf->getDataFifoPipe(), &b, 1); 
  pDataPacket_t newpacket_wave = nf->getNewData(); 

  // one-off data packet test
  BOOST_CHECK_EQUAL(newpacket_raw->seq, dp_raw->seq); 
		    

  // Check each of the data sources individually

  Raw_t new_raw = rawToRaw(newpacket_raw); 
  test_equality(generateCanonicalRaw(src, 0), new_raw); 
  
  TSpike_t new_tspike = rawToTSpike(newpacket_tspike); 
  test_equality(generateCanonicalTSpike(src+1, 1000), new_tspike); 
  
  Wave_t new_wave = rawToWave(newpacket_wave); 
  test_equality(generateCanonicalWave(src+2, 100000), new_wave); 
  

  nf->shutdown(); 
}



BOOST_AUTO_TEST_CASE(test_events_to_soma)
{
  /*
    1. Does libsomanetwork correctly send events to the initially-created
    sockets? 
    2. Does it correctly receive the returned success/failure packets? 

  */ 

  bf::path tempdir = createTempDir(); 

  // standard setup events
  createBoundDomainSocket(tempdir / "dataretx"); 
  createBoundDomainSocket(tempdir / "eventretx"); 
  int eventtxsocket = createBoundDomainSocket(tempdir / "eventtx"); 
  
  int sendingsocket = createBoundDomainSocket(tempdir / "test_sendsocket"); 

  pNetworkInterface_t nf = Network::createDomain(tempdir); 
  
  nf->run(); 
  EventTX_t etx; 
  etx.event.cmd = 0x10; 
  etx.event.src = 0x20; 
  etx.event.data[0] = 0x1234; 
  etx.event.data[4] = 0xAABB; 
  etx.destaddr[0] = true; 
  etx.destaddr[30] = true; 
  
  EventTXList_t etxl; 
  etxl.push_back(etx); 

  nf->sendEvents(etxl); 
  
  // now read the event on the socket
  std::vector<char> buffer(1000); 
  sockaddr_un sa; 
  socklen_t sl = sizeof(sa); 
  ssize_t rf = recvfrom(eventtxsocket, &buffer[0], buffer.size(), 0, 
			(sockaddr*)&sa, &sl); 

  EventTXList_t rxlist; 
  eventtxnonce_t nonce = getEventListFromBuffer(buffer, &rxlist); 

  std::vector<char> responsebuf(4); 
  eventtxnonce_t netnonce = htons(nonce); 
  memcpy(&responsebuf[0], &netnonce, sizeof(netnonce)); 
  responsebuf[2] = (char)1; 
  responsebuf[3] = (char)1; 
  
  // send that nonce back with a "success"
  sendto(eventtxsocket, &responsebuf[0], 4, 0, 
	 (sockaddr*)&sa, sl); 
    
  // send a second event list to make sure we're past the success/failure of 
  // the last packet
  nf->sendEvents(etxl); 
  sl = sizeof(sa); 
  rf = recvfrom(eventtxsocket, &buffer[0], buffer.size(), 0, 
		(sockaddr*)&sa, &sl); 
  rxlist.clear(); 
  nonce = getEventListFromBuffer(buffer, &rxlist); 
  
  nf->shutdown(); 
}


BOOST_AUTO_TEST_CASE(test_enable_disable)
{
  /*
    Can we disable and reenable a particular data source
  */ 
  std::cout << "test_enable_disable" << std::endl; 
  bf::path tempdir = createTempDir(); 
  BOOST_CHECK_THROW(Network::createDomain(tempdir), 
		    std::runtime_error); 
  
  createBoundDomainSocket(tempdir / "dataretx"); 
  createBoundDomainSocket(tempdir / "eventretx"); 
  createBoundDomainSocket(tempdir / "eventtx"); 
  
  int sendingsocket = createBoundDomainSocket(tempdir / "test_sendsocket"); 

  pNetworkInterface_t nf = Network::createDomain(tempdir); 
  
  datasource_t src = 10;
  nf->enableDataRX(src, RAW); 
  nf->disableDataRX(src, RAW); 
  std::cout << "About to reenable" << std::endl; 
  nf->enableDataRX(src, RAW); 
  std::cout << "About to reenable: done" << std::endl; 

  nf->run(); 
  


  nf->shutdown(); 
}

BOOST_AUTO_TEST_SUITE_END(); 

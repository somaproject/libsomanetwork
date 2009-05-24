#ifndef TESTS_H
#define TESTS_H

#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <somanetwork/datapacket.h>
#include <queue>

using namespace somanetwork; 

std::vector<char> fakeDataPacket(unsigned int seq, char src, char typ); 

void sendDataPacket(const std::vector<char> & dp, int port);

class FakeDataServer 
{
  /* 
     Fake data server takes in a list of packet sequence IDs to send,
     and sends those packets on startup. Also runs a retransmission
     thread out-of-process, and will respond with a canonical 
     packet as requested. 

     So if we tell the FakeDataServer to send sequences 0-3, 10-12, and
     issue a retransmission request for 4, the retx thread will send 4. 

   */ 

 public:
  FakeDataServer(datatype_t, datasource_t);
  ~FakeDataServer(); 
  void appendSeqsToSend(std::vector<sequence_t>); 
  void appendSeqsToSend(sequence_t x);
  void start(); 
  void shutdown(); 

 private:
  datasource_t src_; 
  datatype_t typ_; 
  int port_; 
  int sendsock_;
  bool running_; 
  boost::thread *  pmainthrd_; 
  boost::thread *  pretxthrd_; 
  std::queue<sequence_t> fifo_; 
  
  void workthread(); 
  void retxthread(); 

};

#endif // TESTS_H

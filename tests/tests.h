#ifndef TESTS_H
#define TESTS_H

#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "datapacket.h"
#include <queue>

using namespace somanetwork; 

std::vector<char> fakeDataPacket(unsigned int seq, char src, char typ); 

void sendDataPacket(const std::vector<char> & dp, int port);

class FakeDataServer 
{
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

#ifndef TESTS_H
#define TESTS_H

#include <vector>
#include <data/rawdata.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <data/rawdata.h>
#include <queue>

std::vector<char> fakeDataPacket(unsigned int seq, char src, char typ); 

void sendDataPacket(const std::vector<char> & dp, int port);

std::pair<eventseq_t, std::vector<char> > eventsetout_t; 

std::map<eventseq_t, std::vector<char> > retxLUT_t; 
class FakeEventServer 
{
 public:
  FakeEventServer(); 
  ~FakeEventServer(); 
  void appendSeqsToSend(eventsetout_t); 
  void start(); 
  void shutdown(); 
 private:

  int port_; 
  int sendsock_;
  bool running_; 
  boost::thread *  pmainthrd_; 
  boost::thread *  pretxthrd_; 
  std::queue<eventsetout_t> fifo_; 
  
  retxLUT_t retxLUT_; 
  void workthread(); 
  void retxthread(); 
  

};

#endif // TESTS_H

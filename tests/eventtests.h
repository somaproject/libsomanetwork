#ifndef EVENTTEST_H
#define EVENTTEST_H

#include <vector>
#include <data/event.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <map>
#include <iostream>
#include <queue>
#include <list>


typedef std::pair<eventseq_t, std::vector<char> > eventsetout_t; 

typedef std::map<eventseq_t, std::vector<char> > retxLUT_t; 

std::list<EventList_t> genEventList(std::vector<char> es);

class FakeEventServer 
{
 public:
  FakeEventServer(); 
  ~FakeEventServer(); 
  void appendSeqsToSend(eventsetout_t es); 

  void start(); 
  void shutdown(); 

  void setSkip(eventseq_t s); 

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

  std::list<eventseq_t> skiplist_; 
  
};

#endif // EVENTTEST_H

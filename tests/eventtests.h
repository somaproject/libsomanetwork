#ifndef EVENTTEST_H
#define EVENTTEST_H

#include <vector>
#include <event.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <map>
#include <iostream>
#include <queue>
#include <list>
#include <eventtx.h>


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
  bool workthreaddone(); 

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
  boost::mutex workdonemutex_; 
  bool workdone_; 

  std::list<eventseq_t> skiplist_; 


};

typedef std::map<eventtxnonce_t, int> skipMap_t; 

class FakeEventRXServer 
{
  /* 
     FakeEventRXServer is designed to simulate a soma box accepting
     events off the wire. 

     Upon construction, we take two std::maps<nonce, int> of nonce, num:
     norxnonce: for each nonce in this map, do not respond num times. This also means that we do not
     put this in the buffer. 

     failnonce: for each nonce in this list, fail (STATUS =0) num times

     This way, ahead of time, we can dictate which nonces we will incorrectly
     respond to. If a nonce is not in one of these maps or its associated number
     in the map is zero, then we simply respond normally. 
     
     EXAMPLE:
     To tell the system that it should not respond the first 3 times 
     we receive a packet with a NONCE of 117, pass a map with
     map[117] = 3

     
  */
  
 public:
  FakeEventRXServer(skipMap_t norx, skipMap_t fail); 
  ~FakeEventRXServer(); 
  
  EventTXList_t getRXevents(); 
  void start(); 
  void shutdown(eventtxnonce_t); 

 private:
  skipMap_t norxMap_; 
  skipMap_t failMap_; 
  void socketSetup(); 
  eventtxnonce_t latestNonce_ ; 

  int port_; 
  int socket_;
  bool running_; 
  boost::thread *  pmainthrd_; 

  EventTXList_t rxevents_; 
  void sendResponse(eventtxnonce_t nonce, bool success, 
		    sockaddr_in * to, socklen_t tolen); 

  void workthread(); 
  boost::mutex startmutex_; 
  boost::condition startcond_;


};



#endif // EVENTTEST_H

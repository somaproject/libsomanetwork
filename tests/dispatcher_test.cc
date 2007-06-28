#include <boost/test/unit_test.hpp>

#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <arpa/inet.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "eventdispatcher.h"
#include <sys/types.h>
#include <sys/socket.h>

using boost::unit_test::test_suite;

typedef std::list<std::vector<char> > buflist_t; 
buflist_t buflist; 

void append(int fd)
{
  std::vector<char> buffer(2048); 
  int n = read(fd, &buffer[0], 2048); 
  buffer.resize(n); 
  
  buflist.push_back(buffer); 


}

BOOST_AUTO_TEST_CASE( simpledispatch )
{
  /* Let's see if we can correctly receive some data
  */ 

  buflist.clear(); 
  
  EventDispatcher ed; 
  int pipes[2]; 
  socketpair(AF_UNIX, SOCK_DGRAM, 0, pipes); 

  int fdw = pipes[1]; 
  int fdr = pipes[0]; 
  
  ed.addEvent(fdr, &append); 
  // now we run in a single-threaded context
  boost::thread thrd(boost::bind(&EventDispatcher::run, &ed)); 
  for (int i = 0; i < 10; i++)
    {
      char x = (char)i; 
      
      send(fdw, &x, 1, 0); 
    }
  while(buflist.size() < 10) {
    // THIS IS NOT THREAD SAFE, but i don't really care too much right now
  }
  ed.halt(); 
  thrd.join(); 
  BOOST_CHECK_EQUAL(buflist.size(), 10); 
  BOOST_CHECK_EQUAL((unsigned int)buflist.front().size(), 1); 

  buflist_t::iterator i; 
  char pos = 0; 
  for (i = buflist.begin(); i != buflist.end(); i++)
    {
      BOOST_CHECK_EQUAL((*i)[0], pos); 
      pos++; 
    }
}


BOOST_AUTO_TEST_CASE( livedelete )
{
  /* Let's see if we can correctly receive some data
  */ 

  buflist.clear(); 
  
  EventDispatcher ed; 
  int pipes[2]; 
  socketpair(AF_UNIX, SOCK_DGRAM, 0, pipes); 

  int fdw = pipes[1]; 
  int fdr = pipes[0]; 
  
  ed.addEvent(fdr, &append); 
  // now we run in a single-threaded context
  boost::thread thrd(boost::bind(&EventDispatcher::run, &ed)); 
  for (int i = 0; i < 10; i++)
    {
      char x = (char)i; 
      send(fdw, &x, 1, 0); 
    }
  while(buflist.size() < 10) {
    // THIS IS NOT THREAD SAFE, but i don't really care too much right now
  }
  
  ed.delEvent(fdr); 
  sleep(1); 
  for (int i = 0; i < 10; i++)
    {
      char x = (char)i; 
      send(fdw, &x, 1, 0); 
    }
  // sleep for a second
  sleep(1); 
  ed.halt(); 
  thrd.join(); 
  BOOST_CHECK_EQUAL(buflist.size(), 10); 
  BOOST_CHECK_EQUAL((unsigned int)buflist.front().size(), 1); 

  buflist_t::iterator i; 
  char pos = 0; 
  for (i = buflist.begin(); i != buflist.end(); i++)
    {
      BOOST_CHECK_EQUAL((*i)[0], pos); 
      pos++; 
    }
}


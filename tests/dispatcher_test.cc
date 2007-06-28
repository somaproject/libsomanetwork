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

std::list<std::vector<char> > buflist; 

void append(int fd)
{
  std::vector<char> buffer(2048); 
  int n = read(fd, &buffer[0], 2048); 
  buffer.resize(n); 
  
  buflist.push_back(buffer); 


}

BOOST_AUTO_TEST_CASE( simpledispatch )
{
  buflist.clear(); 
  

  /* Let's see if we can correctly receive some data

  
  */ 
  
  
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
  while(buflist.size() < 5) {
  }
  ed.halt(); 
  thrd.join(); 
  std::cout << buflist.size() << std::endl; 
  std::cout << buflist.front().size() << std::endl;

}

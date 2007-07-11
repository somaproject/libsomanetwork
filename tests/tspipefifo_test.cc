#include <boost/test/unit_test.hpp>
#include "tspipefifo.h"
#include <boost/test/auto_unit_test.hpp>
#include <boost/thread/thread.hpp>

#include <boost/bind.hpp>
#include <iostream>
#include <boost/array.hpp>

using boost::unit_test::test_suite;

BOOST_AUTO_TEST_CASE(test )
{

  TSPipeFifo<int> tp; 
  for (int i = 0; i < 10; i++)
    {
    tp.append(i); 
    }
  
  for (int i = 0; i < 10; i++)
    {
      char x; 
      read(tp.readingPipe, &x, 1); 
      int y = tp.pop(); 
      BOOST_CHECK_EQUAL(y, i); 
    }
      
}


void addToFifo(TSPipeFifo<int> * fifo, int count) 
{
  for (int i = 0; i < count; i++) 
    {
      fifo->append(i); 
    }
} 

BOOST_AUTO_TEST_CASE(testthread )
{
  TSPipeFifo<int> tp; 
  int N = 100000; 
  
  boost::thread thrd(boost::bind(addToFifo, &tp, N));

  for (int i = 0; i < N; i++) 
    {
      char x; 
      read(tp.readingPipe, &x, 1); 
      int y = tp.pop(); 
      BOOST_CHECK_EQUAL(y, i); 
    }

  
}

#include "somanetwork/tspipefifo.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> //include all types plus i/o

using namespace boost::posix_time;


void addToFifo(TSPipeFifo<int> * fifo, int count) 
{
  for (int i = 0; i < count; i++) 
    {
      fifo->append(i); 
    }
  std::cout << "Write thread done" << std::endl; 
} 

int main()
{
  TSPipeFifo<int> tp; 
  int N = 100000; 

  ptime t1(microsec_clock::local_time());  
  boost::thread thrd(boost::bind(addToFifo, &tp, N));

  for (int i = 0; i < N; i++) 
    {
      char x; 
      read(tp.readingPipe, &x, 1); 
      int y = tp.pop(); 
      assert(y == i); 
    }
  ptime t2(microsec_clock::local_time());
  std::cout << float(N) / (t2 -t1).total_microseconds()*1e6 << " packets / second" << std::endl;
  thrd.join(); 

}

#ifndef TSPIPEFIFO_H
#define TSPIPEFIFO_H

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

template <class T>
class TSPipeFifo
{

 private:
  std::queue<T> fifo_; 
  boost::mutex mutex_; 
  int writingPipe_; 

 public:
  int readingPipe; 
  TSPipeFifo()
    {
      // create pipes
      int pipes[2]; 
      pipe(pipes); 
      
      readingPipe = pipes[0]; 
      writingPipe_ = pipes[1]; 
      
    }

  ~TSPipeFifo()
    {
      //this leaks if you store pointers in it!
      close(writingPipe_); 
      close(readingPipe); 
    }
  

  void append(T x)
    {
      char test; 
      // we just lock the primitive, not the pipe
      // because if we add too much to the primitive and don't write to the 
      // pipe, it's not like the client will pop the (added) data

      boost::mutex::scoped_lock * sl = new boost::mutex::scoped_lock(mutex_);
      fifo_.push(x); 
      delete sl; 

      write(writingPipe_, &test, 1); 

    }
  
  
  T pop(void)
    {
      char dummy; 
 
      boost::mutex::scoped_lock scoped_lock(mutex_);
      T x = fifo_.front(); 
      fifo_.pop(); 
      
      return x; 
    }
  
}; 

#endif // TSPIPEFIFO_H

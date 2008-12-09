#include <iostream>
#include <vector>
#include <sys/time.h>
#include <time.h>
#include <fstream>
#include <unistd.h>
#include <signal.h>

using namespace std; 

int Nglobal(10000); 
int iglobal(0);
std::vector<uint64_t>  dataglobal(Nglobal); 


uint64_t usecdelta(timeval & t1, timeval & t2) {
  uint64_t x = t1.tv_sec; 
  x = x * 1000000; 
  x += t1.tv_usec; 

  uint64_t y = t2.tv_sec; 
  y = y * 1000000; 
  y += t2.tv_usec;

  return y - x; 
  
}

int main()
{

  sigevent s; 
  s.sigev_notify = SIGEV_SIGNAL; 
  
  sys_timer_create(CLOCK_REALTIME, &s, 

}
int main_test_1()
{
  int N = 100; 
  std::vector<uint64_t>  data(N); 
  
  timeval lasttime; 
  gettimeofday(&lasttime, NULL); 
  
  for(int i = 0; i < N; i++) {
    timeval curtime; 
    timespec ts; 
    ts.tv_sec = 0; 
    ts.tv_nsec = 1000;
    nanosleep(&ts, &ts); 
    gettimeofday(&curtime, NULL); 
    
    uint64_t delta = usecdelta(lasttime, curtime); 
    data[i] = delta; 

    lasttime.tv_sec = curtime.tv_sec; 
    lasttime.tv_usec = curtime.tv_usec; 
  }
  
  uint64_t sum(0); 
  // dump to disk 
  ofstream timesFile; 
  timesFile.open ("time.dat", ios::out | ios::binary); 
  
  for (int i = 0; i < N; i++) {

    timesFile.write((char *)&data[i], sizeof(data[i])); 
  }
  
}

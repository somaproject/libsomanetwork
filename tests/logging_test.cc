
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>    
#include <fstream>      
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include "boost/date_time/posix_time/posix_time.hpp" //include all types plus i/o
#include <sys/time.h>
#include <unistd.h>
#include "test_config.h"
#include <somanetwork/logging.h>

using namespace boost::posix_time;

using namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(logging_test); 

int waitfunc(){
  sleep(1); 

  return 0; 
}

// BOOST_AUTO_TEST_CASE(logging_init) 
// {
//   log4cpp::Priority::Value loglevel = log4cpp::Priority::ERROR; 

//   log4cpp::Appender * logappender =  new log4cpp::OstreamAppender("appender", &std::cout); 
//   log4cpp::Layout* loglayout = new log4cpp::BasicLayout();
//   logappender->setLayout(loglayout); 
//   log4cpp::Category::getRoot().setPriority(loglevel); 
//   log4cpp::Category& root = log4cpp::Category::getRoot();
  
//   root.addAppender(logappender); 

//   std::string LOGROOT("test.log"); 
//   log4cpp::Category& logtest = log4cpp::Category::getInstance(LOGROOT);

//   int N = 10; 
//   int sum = 0; 
//   timeval t1, t2; 
//   gettimeofday(&t1, NULL); 
//   for (int i = 0; i < N; i++) {
//     sum += i; 
//     logtest.infoStream() << " test" <<  waitfunc(); 
//   }
//   gettimeofday(&t2, NULL); 
//   long us = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
//   std::cout << "time = " << us << " us." << std::endl; 

//   std::cout << "sum = " << sum << std::endl; 
// }


BOOST_AUTO_TEST_CASE(logging_take2) 
{
  /*
    Measure overhead. 
   */
  somanetwork::init_logs(); 
  int N = 10000000; 
  int sum = 0; 
  timeval t1, t2; 
  gettimeofday(&t1, NULL); 

  using namespace boost::logging;
  somanetwork_log_level()->set_enabled(level::debug);

  for (int i = 0; i < N; i++) {
    sum += i; 
    L_(debug) << "this will not be written anywhere";
  }

  gettimeofday(&t2, NULL); 
  long us = (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
  std::cout << "time = " << us << " us." << std::endl; 

  std::cout << "sum = " << sum << std::endl; 
  
}



BOOST_AUTO_TEST_SUITE_END(); 

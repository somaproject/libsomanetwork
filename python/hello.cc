#include <boost/python.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include <iostream> 

using namespace boost::python;

char const* greet()
{
  
   return "hello, world";
}


std::string freeze()
{
  // This relies on their being a named pipe, 
  // "pipe", in the working dir
  // 
  // you can do this with mkfifo pipe
  
  FILE * fid =  fopen("pipe", "r"); 
  char result[10]; 

  fread(result, 10, 1, fid); 

  result[9] = 0; 
  return std::string( result); 
}

std::string freeze2()
{
  // This relies on their being a named pipe, 
  // "pipe", in the working dir
  // 
  char result[10]; 

  // you can do this with mkfifo pipe
  Py_BEGIN_ALLOW_THREADS; 

  FILE * fid =  fopen("pipe", "r"); 

  fread(result, 10, 1, fid); 
  result[9] = 0;   
  fclose(fid); 

  Py_END_ALLOW_THREADS; 

  
  return std::string(result); 
}


BOOST_PYTHON_MODULE(hello)
{
  PyEval_InitThreads();

  def("greet", greet);
  def("freeze", freeze); 
  def("freeze2", freeze2); 

}


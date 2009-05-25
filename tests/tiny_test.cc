
#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>    
#include <fstream>                    

using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 


BOOST_AUTO_TEST_SUITE(tiny_test); 

BOOST_AUTO_TEST_CASE(simple)
{
  BOOST_CHECK_EQUAL(1, 1); 
  
}


BOOST_AUTO_TEST_SUITE_END(); 

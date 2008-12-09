#ifndef RANGE_H
#define RANGE_H

#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"  
#include "boost/algorithm/string/split.hpp" 
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <string>
#include <vector>

using namespace std; 

vector<int> parserange(std::string rangestr); 


#endif // RANGE_H

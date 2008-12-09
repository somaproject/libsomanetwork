#include "range.h" 

vector<int> parserange(std::string rangestr)
{
  /*
    turn 0 into [0]
    and 1-3 into [1, 2, 3]
  */
  std::vector<string> strresults; 
  boost::split(strresults, rangestr, boost::is_any_of("-")); 
  std::vector<int> results; 
  if (strresults.size() == 1) {
    // single chan
    results.push_back(atoi(strresults[0].c_str())); 
  } else {
    int start = atoi(strresults[0].c_str()); 
    int end = atoi(strresults[1].c_str()); 
    for (int i = start; i <= end; i++) {
      results.push_back(i); 
    }
  }
  return results; 
}

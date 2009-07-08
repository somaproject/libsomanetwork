#include "somanetwork/logging.h"
#include <boost/logging/format.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/named_write.hpp>

namespace somanetwork {

  using namespace boost::logging;
  
  void init_logs()  {
    g_l()->writer().write("%time%($hh:$mm.$ss.$mili) [%idx%] |\n", "cout file(out.txt) debug");
    g_l()->mark_as_initialized();


  }

}



#include "somanetwork/logging.h"
#include <boost/logging/format.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/named_write.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/formatter/high_precision_time.hpp>

  //using namespace boost::logging;
namespace bl = boost::logging; 



BOOST_DEFINE_LOG_FILTER(somanetwork_log_level, somanetwork_finder::filter ) // holds the application log level
BOOST_DEFINE_LOG(somanetwork_l, somanetwork_finder::logger); 

namespace somanetwork {
  void init_logs( boost::logging::level::type level) {
    somanetwork_log_level()->set_enabled(level); 
    somanetwork_l()->writer().add_formatter(bl::formatter::high_precision_time("$mm.$ss:$micro ")); 
    somanetwork_l()->writer().add_formatter(bl::formatter::append_newline()); 
    somanetwork_l()->writer().add_destination( bl::destination::cout() );
    somanetwork_l()->mark_as_initialized();

  }

}



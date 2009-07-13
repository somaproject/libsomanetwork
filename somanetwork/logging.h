#ifndef SOMANETWORK_LOGGING_H
#define SOMANETWORK_LOGGING_H

#include <boost/logging/format_fwd.hpp>
#include <boost/logging/format/named_write.hpp>

using namespace boost::logging::scenario::usage;
typedef use<
        //  the filter is always accurate (but slow)
        filter_::change::always_accurate, 
        //  filter does not use levels
        filter_::level::use_levels, 
        // the logger is initialized once, when only one thread is running
        logger_::change::set_once_when_one_thread, 
        // the logger favors speed (on a dedicated thread)
        logger_::favor::speed> somanetwork_finder;



#define L_(lvl) BOOST_LOG_USE_LOG_IF_LEVEL(somanetwork_l(), somanetwork_log_level(), lvl )

BOOST_DECLARE_LOG_FILTER(somanetwork_log_level, somanetwork_finder::filter ) // holds the application log level
BOOST_DECLARE_LOG(somanetwork_l, somanetwork_finder::logger)

namespace somanetwork
{
  void init_logs( boost::logging::level::type level); 

}


#endif // SOMANETWORK_LOGGING_H

#ifndef SOMANETWORK_LOGGING_H
#define SOMANETWORK_LOGGING_H

#include <boost/logging/format_fwd.hpp>
#include <boost/logging/format/named_write_fwd.hpp>
#include <boost/logging/format/named_write.hpp>
#include <boost/logging/logging.hpp>
#include <boost/logging/writer/on_dedicated_thread.hpp> 
#include <boost/logging/format_fwd.hpp>


namespace somanetwork { 
  namespace bl = boost::logging; 

  typedef boost::logging::named_logger<>::type logger_type;

#define L_(lvl) BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_log_level(), lvl )


  BOOST_DEFINE_LOG_FILTER(g_log_level, boost::logging::level::holder ) // holds the application log level
  BOOST_DEFINE_LOG(g_l, logger_type)


  void init_logs(); 

}


#endif // SOMANETWORK_LOGGING_H

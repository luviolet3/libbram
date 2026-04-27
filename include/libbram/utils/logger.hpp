#ifndef LB_LOGGER_HPP
#define LB_LOGGER_HPP

#include <ostream>
#include <tree_sitter/api.h>

namespace lb {
  namespace Utils {
    namespace Logger {
      /// Levels of severity for a log
      /// 
      /// TRACE:
      /// DEBUG: 
      /// INFO: 
      /// WARN: 
      /// ERROR: 
      /// FATAL: 
      enum class Level {
        TRACE,
        DEBUG,
        VERBOSE,
        INFO,
        WARNING,
        ERROR,
      };

      /// Sets the output stream to log to. 
      /// Giving no arguments results in turning off logging
      ///
      /// @param ostream the stream to output the log to
      void setOutputStream(std::ostream &ostream);
      void setOutputStream();

      /// Logs a message at a level
      ///
      /// @param level the severtity to log at
      /// @param format string specifying how to interpret the data
      void log(Level level, const char* format, ...);

      void tslog(void *payload, TSLogType type, const char *msg);

      /// Logger for tree-sitter
      inline TSLogger tslogger = { .payload = NULL, .log = tslog };
    }
  }
}

#endif
#ifndef LB_LOGGER_HPP
#define LB_LOGGER_HPP

#include <ostream>

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
      /// By default this is set to standard log (std::clog)
      ///
      /// @param ostream the stream to output the log to
      void setOutputStream(std::ostream ostream);

      /// Logs a message at a level
      ///
      /// @param level the severtity to log at
      /// @param format string specifying how to interpret the data
      void log(Level level, const char* format, ...);

      /// Increment indent level of logs by 1
      void indent();

      /// Decrement indent level of logs by 1
      void unindent();

      /// Flushes all indent operations
      void flush();
    }
  }
}

#endif
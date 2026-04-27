#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "utils/logger.hpp"

namespace lb {
  namespace Utils {
    namespace Logger {
      namespace {
        std::ostream *out = nullptr;

        #ifdef LOG_MIN_LEVEL
        Level minLevel = Level::LOG_MIN_LEVEL;
        #elif !defined NDEBUG
        Level minLevel = Level::DEBUG;
        #else
        Level minLevel = Level::INFO;
        #endif
        
        /// Prints the level with coloring using ANSI codes
        ///
        /// @param level the log level to format
        void printLevel(Level level) {
          if (out == nullptr) return;
          switch (level) {
            case Level::TRACE:
              *out << "[TRACE] ";
              break;
            case Level::DEBUG:
              *out << "[DEBUG] ";
              break;
            case Level::VERBOSE:
            case Level::INFO:
              *out << "[INFO ] ";
              break;
            case Level::WARNING:
              *out << "\e[33m[WARN ]\e[0m ";
              break;
            case Level::ERROR:
              *out << "\e[31m[ERROR]\e[0m ";
              break;
            default:
              *out << "[OTHER] ";
              break;
          }
        }
        
        /// Prints the text, indenting and prepending the time and level information
        ///
        /// @param level the severity to print at
        /// @param text the text to print
        void print(Level level, const char* text) {
          if (out == nullptr) return;
          std::time_t t = std::time(nullptr);
          std::stringstream textStream(text);
          std::string line;
          if (getline(textStream, line)) {
            *out << std::put_time(std::localtime(&t), "%H:%M:%S ");
            printLevel(level);
            // printIndent();
            *out << line << std::endl;
          }

          while (getline(textStream, line)) {
            *out << "                 ";
            // printIndent();
            *out << line << std::endl;
          }
        }
      }

      void setOutputStream(std::ostream &ostream) {
        out = &ostream;
      }

      void log(Level level, const char* format, ...) {
        if (!out) return;
        if (level < minLevel) return;

        std::va_list args;
        va_start(args, format);
        int len = vsnprintf(NULL, 0, format, args) + 2;
        va_end(args);
        char *buf = (char*)calloc(len, sizeof(char));
        va_start(args, format);
        vsprintf(buf, format, args);
        va_end(args);
        print(level, buf);
        free(buf);
      }

      void tslog(void *payload, TSLogType type, const char *msg) {
        log(Level::TRACE, "%s: %s\n", type == TSLogTypeParse ? "Parse" : "Lex", msg);
      }
    }
  }
}
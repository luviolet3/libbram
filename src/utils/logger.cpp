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
        const char* INDENT = "│ ";
        const char* INDENT_CHILD = "├─";
        const char* INDENT_CHILD_END = "╰─";
        const char* INDENT_CHILD_ORPHAN = "┬─";

        std::ostream *out = &std::clog;
        unsigned int curIndent = 0;
        int nextIndent = 0;
        #ifdef LOG_MIN_LEVEL
        Level minLevel = Level::LOG_MIN_LEVEL;
        #elif !defined NDEBUG
        Level minLevel = Level::TRACE;
        #else
        Level minLevel = Level::INFO;
        #endif
        
        /// Prints the level with coloring using ANSI codes
        ///
        /// @param level the log level to format
        void printLevel(Level level) {
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

        /// Prints the formatted indent level
        void printIndent() {
          if (curIndent == 0) return;
          for (int i = 1; i < curIndent; i++)
            *out << INDENT;
          *out << INDENT_CHILD;
        }
        
        /// Prints the text, indenting and prepending the time and level information
        ///
        /// @param level the severity to print at
        /// @param text the text to print
        void print(Level level, const char* text) {
          std::time_t t = std::time(nullptr);
          std::stringstream textStream(text);
          std::string line;
          if (getline(textStream, line)) {
            *out << std::put_time(std::localtime(&t), "%H:%M:%S ");
            printLevel(level);
            printIndent();
            *out << line << std::endl;
          }

          while (getline(textStream, line)) {
            *out << "                 ";
            printIndent();
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
        flush();

        std::va_list args;
        va_start(args, format);
        int len = vsnprintf(NULL, 0, format, args) + 2;
        va_end(args);
        char *buf = (char*)calloc(len, sizeof(char));
        va_start(args, format);
        vsprintf(buf, format, args);
        va_end(args);
        print(level, buf);
      }

      void indent() { if (++nextIndent > 1) nextIndent = 1; }
      void unindent() { if (--nextIndent < -1) nextIndent = -1; }
      void flush() {
        if (nextIndent > 0) {
          curIndent++;
          nextIndent = 0;
        } else if (nextIndent < 0) {
          if (curIndent > 0) {
            int len = snprintf(NULL, 0, "\e[1F\e[%dG%s\e[1E", curIndent*((int)strlen(INDENT)-2) + 16, INDENT_CHILD_END);
            char *buf = (char*)calloc(len, sizeof(char));
            sprintf(buf, "\e[1F\e[%dG%s\e[1E", curIndent*((int)strlen(INDENT)-2) + 16, INDENT_CHILD_END);
            *out << buf;
          }
          curIndent--;
          nextIndent = 0;
        }
      }
    }
  }
}
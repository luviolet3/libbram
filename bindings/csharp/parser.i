%module base_module

%{
#include "parser.hpp"
%}

namespace lb {
  namespace Parser {
    void parse(const char* file);
  }
}
%module base_module

%{
#include "parser.hpp"
%}

namespace lb {
  class Parser {
    public:
    static void *parse(const char* file);
   };
}
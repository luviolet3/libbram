#ifndef LB_PARSER_HPP
#define LB_PARSER_HPP

#include <unordered_map>

#include "rapidxml-1.13/rapidxml.hpp"

namespace lb {
  namespace Parser {
    void *parse(const char* file);
  }
}

#endif
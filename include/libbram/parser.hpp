#ifndef LB_PARSER_HPP
#define LB_PARSER_HPP

#include "bram.hpp"
#include <string>

namespace lb {
  namespace Parser {

    /// Parses the contents of a dotbram file
    ///
    /// @param contents the contents to parse
    BramFile *parse(const std::string &contents);

    /// Parses the contents of a dotbram file
    ///
    /// @param file the file to parse
    /// @param log if logs should be written. Defaults to false
    BramFile *parseFile(const std::string &file, bool log=false);
  }
}

#endif
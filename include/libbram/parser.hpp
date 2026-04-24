#ifndef LB_PARSER_HPP
#define LB_PARSER_HPP

namespace lb {
  namespace Parser {
    /// Parses the contents of a dotbram file
    ///
    /// @param contents the contents to parse
    void parse(const char* contents);

    /// Parses the contents of a dotbram file
    ///
    /// @param file the file to parse
    void parseFile(const char* file);
  }
}

#endif
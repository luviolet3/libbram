%module(package="libbram", docstring="Various tools for parsing .bram files") parser
%include "std_string.i"

%{
#include "parser.hpp"
#include "bram.hpp"
%}

%rename(parse) lb::Parser::parse;
%rename(parseFile) lb::Parser::parseFile;

namespace lb {
  namespace Parser {
    /// Parses the contents of a dotbram file
    ///
    /// @param contents the contents to parse
    lb::BramFile *parse(const std::string &contents);

    /// Parses the contents of a dotbram file
    ///
    /// @param file the file to parse
    lb::BramFile *parseFile(const std::string &file, bool log=false);
  }
}
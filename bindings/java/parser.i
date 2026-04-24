%module libbram

%{
#include "parser.hpp"
%}

%rename(parse) lb::Parser::parse;
%rename(load_from_file) lb::Parser::parseFile;

%include "parser.hpp"
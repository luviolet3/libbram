%module libbram

%{
#include "parser.hpp"
%}
 
%rename(parse) lb::Parser::parse;
%rename(load) lb::Parser::parseFile;

%include "parser.hpp"
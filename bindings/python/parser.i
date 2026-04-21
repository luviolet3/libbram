%module base_module

%{
#include "parser.hpp"
%}

%rename(Parserparse) lb::Parser::parse;

%include "parser.hpp"
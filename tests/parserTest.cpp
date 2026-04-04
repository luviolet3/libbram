#include "parser.hpp"

#include "rapidxml-1.13/rapidxml_utils.hpp"

#include <iostream>

int main() {
  #ifdef NDEBUG
  std::cout << "NDEBUG" << std::endl;
  #else
  std::cout << "DEBUG" << std::endl;
  #endif

  rapidxml::file<> xmlFile("tests/example.bram");
  lb::Parser::parse(xmlFile.data());
}
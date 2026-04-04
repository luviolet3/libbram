#include "parser.hpp"

#include "rapidxml-1.13/rapidxml_utils.hpp"

int main() {
  rapidxml::file<> xmlFile("tests/example.bram");
  lb::Parser::parse(xmlFile.data());
}
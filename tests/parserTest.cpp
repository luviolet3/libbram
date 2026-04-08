#include "parser.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "utils/logger.hpp"

int main() {
  try {
    rapidxml::file<> xmlFile("tests/example.bram");
    lb::Parser::parse(xmlFile.data());
  } catch (std::exception& e) {
    lb::Utils::Logger::log(lb::Utils::Logger::Level::ERROR, e.what());
  }
}
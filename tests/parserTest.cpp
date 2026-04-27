#include "parser.hpp"
#include "utils/logger.hpp"

int main(int argc, char **argv) {
  try {
    if (argc == 1)
      lb::Parser::parseFile("tests/example.bram", true);
    else
      lb::Parser::parseFile(argv[1], true);
  } catch (std::exception& e) {
    lb::Utils::Logger::log(lb::Utils::Logger::Level::ERROR, e.what());
  }
}
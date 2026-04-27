#include "parser.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "utils/logger.hpp"

#define TEST(testfile) \
  do { try {                                                            \
    lb::Parser::parseFile(testfile, true);                              \
  } catch (std::exception& e) {                                         \
    lb::Utils::Logger::log(lb::Utils::Logger::Level::ERROR, e.what());  \
  } } while (0)

int main() {
   TEST("tests/pred-proof-1.bram");
   TEST("tests/pred-proof-2.bram");
   TEST("tests/pred-proof-3.bram");
   TEST("tests/pred-proof-4.bram");
   TEST("tests/pred-proof-5.bram");
   TEST("tests/pred-proof-6.bram");
   TEST("tests/pred-proof-7.bram");
   TEST("tests/pred-proof-8.bram");
   TEST("tests/pred-proof-9.bram");
   TEST("tests/resolution-example.bram");
}

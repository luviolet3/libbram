cmake_minimum_required(VERSION 4.2)
project(libbram)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_executable(ParserTest)

add_library(Parser)
add_library(RapidXML)


target_link_libraries(ParserTest PRIVATE Parser)
target_link_libraries(Parser PRIVATE RapidXML)
target_link_libraries(ParserTest PRIVATE RapidXML)

target_sources(ParserTest
  PRIVATE
    tests/parserTest.cpp
)

target_sources(Parser
  PRIVATE
    src/parser.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      src
    FILES
      src/parser.hpp
)

target_sources(RapidXML
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      lib
    FILES
      lib/rapidxml-1.13/rapidxml.hpp
      lib/rapidxml-1.13/rapidxml_utils.hpp
)
set_target_properties(RapidXML PROPERTIES LINKER_LANGUAGE CXX)
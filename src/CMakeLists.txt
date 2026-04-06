add_library(Parser)

target_link_libraries(Parser
  PRIVATE
    RapidXML
    tree-sitter-bram
    cpp-tree-sitter
)

target_sources(Parser
  PRIVATE
    parser.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      ../include/libbram
    FILES
      ../include/libbram/parser.hpp
)
add_library(Logger)
target_link_libraries(Logger
  PRIVATE
)
target_sources(Logger
  PRIVATE
    utils/logger.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      ../include/libbram
    FILES
      ../include/libbram/utils/logger.hpp
)

add_library(Parser)
target_link_libraries(Parser
  PRIVATE
    AST
    cpp-tree-sitter
    Logger
    RapidXML
    tree-sitter-bram
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

add_library(AST)
target_link_libraries(AST
  PRIVATE
    cpp-tree-sitter
    Logger
)
target_sources(AST
  PRIVATE
    ast.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      ../include/libbram
    FILES
      ../include/libbram/ast.hpp
)
set_target_properties(AST PROPERTIES LINKER_LANGUAGE CXX)
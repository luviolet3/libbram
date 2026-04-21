#find_package(Python3 REQUIRED COMPONENTS Development)
#include_directories(${Python3_INCLUDE_DIRS})
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

set_source_files_properties(parser.i PROPERTIES CPLUSPLUS ON SWIG_FLAGS "-includeall" USE_TARGET_INCLUDE_DIRECTORIES ON)
swig_add_library(Parser_Lua
  LANGUAGE lua
  SOURCES parser.i
)
target_link_libraries(Parser_Lua PRIVATE Parser RapidXML ${Python3_LIBRARIES})

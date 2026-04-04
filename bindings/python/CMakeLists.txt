find_package(Python3 REQUIRED COMPONENTS Development)
include_directories(${Python3_INCLUDE_DIRS})

set_source_files_properties(parser.i PROPERTIES CPLUSPLUS ON SWIG_FLAGS "-includeall")
swig_add_library(Parser_Python
  LANGUAGE python
  SOURCES parser.i
)
target_link_libraries(Parser_Python PRIVATE Parser RapidXML ${Python3_LIBRARIES})

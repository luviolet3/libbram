include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set_source_files_properties(parser.i PROPERTIES CPLUSPLUS ON SWIG_FLAGS "-includeall" USE_TARGET_INCLUDE_DIRECTORIES ON)
swig_add_library(Parser_Go
  LANGUAGE go
  SOURCES parser.i
)
target_link_libraries(Parser_Go PRIVATE Parser RapidXML)

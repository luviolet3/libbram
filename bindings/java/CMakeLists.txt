find_package(JNI)
include_directories(${JAVA_INCLUDE_PATH})

set_source_files_properties(parser.i PROPERTIES CPLUSPLUS ON SWIG_FLAGS "-includeall")
swig_add_library(Parser_Java
  LANGUAGE java
  SOURCES parser.i
)
target_link_libraries(Parser_Java PRIVATE Parser RapidXML JNI::JNI)

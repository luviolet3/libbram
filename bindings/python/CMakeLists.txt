find_package(Python3 REQUIRED COMPONENTS Development)
include_directories(${Python3_INCLUDE_DIRS})

set_source_files_properties(bram.i PROPERTIES CPLUSPLUS ON SWIG_FLAGS "-includeall" USE_TARGET_INCLUDE_DIRECTORIES ON)
swig_add_library(Bram_Python
  LANGUAGE python
  SOURCES bram.i
  #OUTPUT_DIR libbram
)
target_link_libraries(Bram_Python PRIVATE Bram ${Python3_LIBRARIES})


set_source_files_properties(parser.i PROPERTIES CPLUSPLUS ON SWIG_FLAGS "-includeall" USE_TARGET_INCLUDE_DIRECTORIES ON)
swig_add_library(Parser_Python
  LANGUAGE python
  SOURCES parser.i
  #OUTPUT_DIR libbram
)
target_link_libraries(Parser_Python PRIVATE Bram Parser RapidXML ${Python3_LIBRARIES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libbram/__init__.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/libbram)

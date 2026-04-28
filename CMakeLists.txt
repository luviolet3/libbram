cmake_minimum_required(VERSION 4.2)
project(libbram)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set (CMAKE_CXX_STANDARD 20)
message("Building ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g)
else()
  add_compile_options(-DNDEBUG)
endif()

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(tests)

include(cmake/CPM.cmake)
CPMAddPackage(
  NAME cpp-tree-sitter
  GIT_REPOSITORY https://github.com/luviolet3/cpp-tree-sitter.git
  GIT_TAG v0.1.0
)
add_grammar_from_repo(tree-sitter-bram
  https://github.com/luviolet3/tree-sitter-bram.git
  0.3.0
)


#set(LANGUAGE_BINDINGS csharp go java javascript lua python)
set(LANGUAGE_BINDINGS python)
string(REPLACE ";" " " LANGUAGE_BINDINGS_STR "${LANGUAGE_BINDINGS}")
find_package(SWIG OPTIONAL_COMPONENTS LANGUAGE_BINDINGS_STR)
if(SWIG_FOUND)
  message("SWIG found: ${SWIG_EXECUTABLE}")
  include(UseSWIG)
  message("Finding bindings")
  foreach(language ${LANGUAGE_BINDINGS})
    if(IS_DIRECTORY bindings/${language} AND SWIG_${language}_FOUND)
      message("Generate ${language} bindings: true")
      add_subdirectory(bindings/${language})
    else()
      message("Generate ${language} bindings: false")
    endif()
  endforeach()
else()
  message("SWIG not found")
endif()
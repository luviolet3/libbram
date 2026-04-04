add_library(RapidXML)

target_sources(RapidXML
  PUBLIC
    FILE_SET HEADERS
    FILES
      rapidxml-1.13/rapidxml_iterators.hpp
      rapidxml-1.13/rapidxml_print.hpp
      rapidxml-1.13/rapidxml_utils.hpp
      rapidxml-1.13/rapidxml.hpp
)
set_target_properties(RapidXML PROPERTIES LINKER_LANGUAGE CXX)
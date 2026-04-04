add_executable(ParserTest)

target_link_libraries(ParserTest PRIVATE Parser)
target_link_libraries(ParserTest PRIVATE RapidXML)

target_sources(ParserTest
  PRIVATE
    parserTest.cpp
)
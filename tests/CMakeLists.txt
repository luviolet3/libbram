add_executable(ParserTest)

target_link_libraries(ParserTest
  PRIVATE
    Logger
    Parser
    RapidXML
)

target_sources(ParserTest
  PRIVATE
    parserTest.cpp
)
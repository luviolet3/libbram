add_executable(ParserTest)
add_executable(PredTest)

target_link_libraries(ParserTest
  PRIVATE
    Logger
    Parser
    RapidXML
)
target_link_libraries(PredTest
  PRIVATE
    Logger
    Parser
    RapidXML
)

target_sources(ParserTest
  PRIVATE
    parserTest.cpp
)
target_sources(PredTest
  PRIVATE
    predTest.cpp
)

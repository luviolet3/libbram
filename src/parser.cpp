#include "rapidxml-1.13/rapidxml.hpp"
#include "parser.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>

#ifndef NDEBUG
#include <iostream>
#endif

namespace lb {
  void Parser::parse(const char* file) {
    #ifndef NDEBUG
    std::cout << "Copying file for safety: " << std::endl;
    #endif

    std::size_t len = strlen(file);
    #ifndef NDEBUG
    std::cout << "|-File length: " << len << " bytes" << std::endl;
    #endif
    char *text = (char*) calloc(sizeof(char), len);
    text = strcpy(text, file);
    #ifndef NDEBUG
    std::cout << "|-Coppied: " << strlen(text) << " bytes" << std::endl;
    #endif

    #ifndef NDEBUG
    std::cout << "Parsing XML: ";
    #endif

    rapidxml::xml_document<> doc;
    doc.parse<0>(text);

    #ifndef NDEBUG
    std::cout << "Done" << std::endl;
    #endif

    parseProof(doc.first_node("bram")->first_node("proof"));
  }

  void Parser::parseProof(rapidxml::xml_node<> *proof) {
    int id = atoiNullptr(getAttribute(proof, "id"));

    #ifndef NDEBUG
    std::cout << "|-Parsing proof " << id << std::endl;
    #endif

    #ifndef NDEBUG
    std::cout << "| |-Parsing assumptions" << std::endl;
    #endif
    for (rapidxml::xml_node<> *assumption = proof->first_node("assumption"); assumption; assumption = assumption->next_sibling("assumption"))
      parseAssumption(assumption);

    #ifndef NDEBUG
    std::cout << "| |-Parsing steps"<< std::endl;
    #endif
    for (rapidxml::xml_node<> *step = proof->first_node("step"); step; step = step->next_sibling("step"))
      parseStep(step);

    #ifndef NDEBUG
    std::cout << "| |-Parsing goals"<< std::endl;
    #endif
    for (rapidxml::xml_node<> *goal = proof->first_node("goal"); goal; goal = goal->next_sibling("goal"))
      parseGoal(goal);
  }

  /**
    * Parses an assumption
    * 
    * @param assumption the assumption to parse
    *
    * @return the proof
    */
  void Parser::parseAssumption(rapidxml::xml_node<> *assumption) {
    int linenum = getLinenum(assumption);
    #ifndef NDEBUG
    std::cout << "| | |-Parsing assumption on line " << linenum << std::endl;
    #endif

    char* raw = getNode(assumption, "raw");
    #ifndef NDEBUG
    std::cout << "| | | |-raw: \"" << raw << "\"" << std::endl;
    #endif
  }

  /**
    * Parses a step
    * 
    * @param step the step to parse
    *
    * @return the proof
    */
  void Parser::parseStep(rapidxml::xml_node<> *step) {
    int linenum = getLinenum(step);
    #ifndef NDEBUG
    std::cout << "| | |-Parsing step on line " << linenum << std::endl;
    #endif

    char* raw = getNode(step, "raw");
    char* rule = getNode(step, "rule");
    char* premise = getNode(step, "premise");
    #ifndef NDEBUG
    if (raw) std::cout << "| | | |-raw: \"" << raw << "\"" << std::endl;
    std::cout << "| | | |-rule: \"" << rule << "\"" << std::endl;
    if (premise) std::cout << "| | | |-premise: \"" << premise << "\"" << std::endl;
    #endif
  }

  /**
    * Parses a goal
    * 
    * @param goal the goal to parse
    *
    * @return the proof
    */
  void Parser::parseGoal(rapidxml::xml_node<> *goal) {
    int linenum = getLinenum(goal);
    #ifndef NDEBUG
    std::cout << "| | |-Parsing goal on line " << linenum << std::endl;
    #endif

    char* raw = getNode(goal, "raw");
    #ifndef NDEBUG
    std::cout << "| | | |-raw: \"" << raw << "\"" << std::endl;
    #endif
  }

  // Helper functions

  /**
    * Gets the value of a child node with certain name
    * 
    * @param node the node to search in
    * @param name the name of the node to search for
    * 
    * @return the value of the child node if found or nullptr if not found
    */
  char* Parser::getNode(rapidxml::xml_node<> *node, const char* name) {
    return node->first_node(name) ? node->first_node(name)->value() : nullptr;
  }

  /**
    * Gets the value of an attribute with certain name
    * 
    * @param node the node to search in
    * @param name the name of the attribute to search for
    * 
    * @return the value of the attribute if found or nullptr if not found
    */
  char* Parser::getAttribute(rapidxml::xml_node<> *node, const char* name) {
    return node->first_attribute(name) ? node->first_attribute(name)->value() : nullptr;
  }

  /**
    * Converts a string to an integer
    * 
    * @param __nptr the string to convert
    * 
    * @return the value as an integer or -1 if null string
    */
  int Parser::atoiNullptr(const char* __nptr) {
    return __nptr ? atoi(__nptr) : -1;
  }

  /**
    * Gets the line number of a node
    * 
    * @param node the node to get the line number of
    * 
    * @return the line number or -1 if error
    */
  int Parser::getLinenum(rapidxml::xml_node<> *node) {
    return atoiNullptr(getAttribute(node, "linenum"));
  }
}
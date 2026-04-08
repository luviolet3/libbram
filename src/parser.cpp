#include <cpp-tree-sitter.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unordered_map>

#include "ast.hpp"
#include "parser.hpp"
#include "rapidxml-1.13/rapidxml.hpp"
#include "utils/logger.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

namespace lb {
  namespace Parser {
    namespace {
      extern "C" {
        TSLanguage *tree_sitter_bram(void);
      }
      const ts::Language tsBramLanguage = tree_sitter_bram();

      // Helper functions

      /// Gets the value of a child node with certain name
      /// 
      /// @param node the node to search in
      /// @param name the name of the node to search for
      /// 
      /// @return the value of the child node if found or nullptr if not found
      char* getNode(rapidxml::xml_node<> *node, const char* name) {
        return node->first_node(name) ? node->first_node(name)->value() : nullptr;
      }

      /// Gets the value of an attribute with certain name
      /// 
      /// @param node the node to search in
      /// @param name the name of the attribute to search for
      /// 
      /// @return the value of the attribute if found or nullptr if not found
      char* getAttribute(rapidxml::xml_node<> *node, const char* name) {
        return node->first_attribute(name) ? node->first_attribute(name)->value() : nullptr;
      }

      /// Converts a string to an integer
      /// 
      /// @param __nptr the string to convert
      /// 
      /// @return the value as an integer or -1 if null string
      int atoiNullptr(const char* __nptr) {
        return __nptr ? atoi(__nptr) : -1;
      }

      /// Gets the line number of a node
      /// 
      /// @param node the node to get the line number of
      /// 
      /// @return the line number or -1 if error
      int getLinenum(rapidxml::xml_node<> *node) {
        return atoiNullptr(getAttribute(node, "linenum"));
      }


      // Parsers

      /// Parses a raw expression
      /// 
      /// @param raw the raw logic expression to parse
      /// 
      /// @return the AST of the parsed expression
      void parseRaw(const char* raw) {
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing raw \"%s\"", raw);
        Utils::Logger::indent();
        ts::Parser tsParser{tsBramLanguage};

        if (strlen(raw) == 0) {
          Utils::Logger::log(Utils::Logger::Level::WARNING, "Empty raw");
          Utils::Logger::unindent();
          Utils::Logger::flush();
          return;
        }

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Stripping comments");
        std::stringstream rawStream(raw);
        std::string line;
        std::string trimmed = "";
        while (getline(rawStream, line)) {
          trimmed += strtok(line.data(), ";");
        }
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing \"%s\"", trimmed.c_str());
        
        ts::Tree ast = tsParser.parseString(trimmed + "\n");

        ts::Node root = ast.getRootNode();
        Utils::Logger::log(Utils::Logger::Level::DEBUG, "AST: \"%s\"", root.getSExpr().get());

        parseAST(root);

        if (ast.getRootNode().hasError()) {
          Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed raw");
          Utils::Logger::unindent();
          Utils::Logger::flush();
          return;
        }

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing raw done");
        Utils::Logger::unindent();
        Utils::Logger::flush();
      }

      /// Parses an assumption
      /// 
      /// @param assumption the assumption to parse
      /// 
      /// @return the proof
      void parseAssumption(rapidxml::xml_node<> *assumption) {
        int linenum = getLinenum(assumption);
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing assumption on line %d", linenum);
        Utils::Logger::indent();

        char* raw = getNode(assumption, "raw");
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Raw: %s", raw);

        parseRaw(raw);

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing assumption on line %d done", linenum);
        Utils::Logger::unindent();
        Utils::Logger::flush();
      }

      /// Parses a step
      /// 
      /// @param step the step to parse
      /// 
      /// @return the proof
      void parseStep(rapidxml::xml_node<> *step) {
        int linenum = getLinenum(step);
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing step on line %d", linenum);
        Utils::Logger::indent();

        char* raw = getNode(step, "raw");
        if (raw)
          Utils::Logger::log(Utils::Logger::Level::TRACE, "Raw: %s", raw);
        char* rule = getNode(step, "rule");
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Rule: %s", rule);
        char* premise = getNode(step, "premise");
        if (premise)
          Utils::Logger::log(Utils::Logger::Level::TRACE, "Premise: %s", premise);

        if (raw) parseRaw(raw);

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing step on line %d done", linenum);
        Utils::Logger::unindent();
        Utils::Logger::flush();
      }

      /// Parses a goal
      /// 
      /// @param goal the goal to parse
      /// 
      /// @return the proof
      void parseGoal(rapidxml::xml_node<> *goal) {
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing goal");
        Utils::Logger::indent();

        char* raw = getNode(goal, "raw");
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Raw: %s", raw);

        parseRaw(raw);

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing goal done");
        Utils::Logger::unindent();
        Utils::Logger::flush();
      }

      /// Parses a proof
      /// 
      /// @param proof the proof to parse
      /// 
      /// @return the proof
      void parseProof(rapidxml::xml_node<> *proof, std::unordered_map<int, void*> &proofMap) {
        int id = atoiNullptr(getAttribute(proof, "id"));
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing proof %d", id);
        Utils::Logger::indent();

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing assumptions");
        Utils::Logger::indent();
        for (rapidxml::xml_node<> *assumption = proof->first_node("assumption"); assumption; assumption = assumption->next_sibling("assumption"))
          parseAssumption(assumption);
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing assumptions done");
        Utils::Logger::unindent();

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing steps");
        Utils::Logger::indent();
        for (rapidxml::xml_node<> *step = proof->first_node("step"); step; step = step->next_sibling("step"))
          parseStep(step);
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing steps done");
        Utils::Logger::unindent();

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing goals");
        Utils::Logger::indent();
        for (rapidxml::xml_node<> *goal = proof->first_node("goal"); goal; goal = goal->next_sibling("goal"))
          parseGoal(goal);
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing goals done");
        Utils::Logger::unindent();

        proofMap[id] = nullptr;

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing proof %d done", id);
        Utils::Logger::unindent();
        Utils::Logger::flush();
      }
    }

    /// Parses the contents of a dotbram file
    ///
    /// @param contents the contents to parse
    void parse(const char* contents) {
      Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing contents");
      Utils::Logger::indent();
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Copying contents for memory safety");
      Utils::Logger::indent();
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Buffer length: %lu bytes", strlen(contents));
      char *text = strdup(contents);
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Coppied: %lu bytes", strlen(contents));
      Utils::Logger::unindent();

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing XML");
      rapidxml::xml_document<> doc;
      doc.parse<0>(text);
      rapidxml::xml_node<> *bram = doc.first_node("bram");
      if (!bram) {
        Utils::Logger::log(Utils::Logger::Level::ERROR, "Malformed XML: bram node not found");
        Utils::Logger::unindent();
        Utils::Logger::flush();
        return;
      }

      Utils::Logger::log(Utils::Logger::Level::INFO, "Getting metadata");
      Utils::Logger::indent();
      const char* program = getNode(bram, "program");
      if (!program)
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: program node not found");
      if (strlen(program) == 0) {
        program = nullptr;
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: program empty");
      }
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Program: %s", program);
      const char* version = getNode(bram, "version");
      if (!version)
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: version node not found");
      if (strlen(version) == 0) {
        version = nullptr;
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: version empty");
      }
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Version: %s", version);
      const char* author = getNode(bram->first_node("metadata"), "author");
      if (!author)
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: author node not found");
      if (strlen(author) == 0) {
        author = nullptr;
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: author empty");
      }
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Author: %s", author);
      Utils::Logger::unindent();

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing proofs");
      Utils::Logger::indent();
      std::unordered_map<int, void*> proofMap;
      for (rapidxml::xml_node<> *proof = bram->first_node("proof"); proof; proof = proof->next_sibling("proof")) {
        parseProof(proof, proofMap);
      }
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing proofs done");
      Utils::Logger::unindent();

      Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing done");
      Utils::Logger::unindent();
      Utils::Logger::flush();
    }
  }
}
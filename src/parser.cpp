#include <cpp-tree-sitter.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unordered_map>

#include "parser.hpp"
#include "rapidxml-1.13/rapidxml.hpp"

#ifndef NDEBUG
#include <iostream>
#endif

namespace lb {
  namespace Parser {
    namespace {
      extern "C" {
        TSLanguage *tree_sitter_bram(void);
        const ts::Language tsBramLanguage = tree_sitter_bram();

        // Helper functions

        /**
          * Gets the value of a child node with certain name
          * 
          * @param node the node to search in
          * @param name the name of the node to search for
          * 
          * @return the value of the child node if found or nullptr if not found
          */
        char* getNode(rapidxml::xml_node<> *node, const char* name) {
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
        char* getAttribute(rapidxml::xml_node<> *node, const char* name) {
          return node->first_attribute(name) ? node->first_attribute(name)->value() : nullptr;
        }

        /**
          * Converts a string to an integer
          * 
          * @param __nptr the string to convert
          * 
          * @return the value as an integer or -1 if null string
          */
        int atoiNullptr(const char* __nptr) {
          return __nptr ? atoi(__nptr) : -1;
        }

        /**
          * Gets the line number of a node
          * 
          * @param node the node to get the line number of
          * 
          * @return the line number or -1 if error
          */
        int getLinenum(rapidxml::xml_node<> *node) {
          return atoiNullptr(getAttribute(node, "linenum"));
        }


        // Parsers

        /**
         * Parses a raw expression
         * 
         * @param raw the raw logic expression to parse
         *
         * @return the AST of the parsed expression
         */
        void parseRaw(const char* raw) {
          ts::Parser tsParser{tsBramLanguage};
          #ifndef NDEBUG
          std::cout << "│ │ │ ├─Creating AST" << std::endl;
          #endif

          if (strlen(raw) == 0) {
            #ifndef NDEBUG
            std::cout << "│ │ │ │ ├─Empty expression" << std::endl;
            std::cout << "│ │ │ │ ├─Aborting" << std::endl;
            #endif
            return;
          }

          #ifndef NDEBUG
          std::cout << "│ │ │ │ ├─Stripping comments" << std::endl;
          #endif
          std::stringstream rawStream(raw);
          std::string line;
          std::string trimmed = "";
          while (getline(rawStream, line)) {
            trimmed += strtok(line.data(), ";");
          }
          #ifndef NDEBUG
          std::cout << "│ │ │ │ ├─Stripped: \"" << trimmed << "\"" << std::endl;
          std::cout << "│ │ │ │ ├─Parsing \"" << trimmed << "\"" << std::endl;
          #endif
          
          trimmed += "\n";
          ts::Tree ast = tsParser.parseString(trimmed);

          #ifndef NDEBUG
          std::cout << "│ │ │ │ ├─Parsed" << std::endl;
          #endif

          ts::Node root = ast.getRootNode();
          
          #ifndef NDEBUG
          std::cout << "│ │ │ │ ├─Verifying AST" << std::endl;
          #endif

          if (ast.getRootNode().hasError()) {
            #ifndef NDEBUG
            std::cout << "│ │ │ │ ├─Malformed raw" << std::endl;
            std::cout << "│ │ │ │ ├─Aborting" << std::endl;
            #endif
            return;
          }

          #ifndef NDEBUG
          std::cout << "│ │ │ │ ├─Verified" << std::endl;
          std::cout << "│ │ │ │ ╰─AST: " << root.getSExpr().get() << std::endl;
          #endif
        }

        /**
         * Parses an assumption
         * 
         * @param assumption the assumption to parse
         *
         * @return the proof
         */
        void parseAssumption(rapidxml::xml_node<> *assumption) {
          int linenum = getLinenum(assumption);
          #ifndef NDEBUG
          std::cout << "│ │ ├─Parsing assumption on line " << linenum << std::endl;
          #endif

          char* raw = getNode(assumption, "raw");
          #ifndef NDEBUG
          std::cout << "│ │ │ ├─raw: \"" << raw << "\"" << std::endl;
          #endif

          parseRaw(raw);
        }

        /**
         * Parses a step
         * 
         * @param step the step to parse
         *
         * @return the proof
         */
        void parseStep(rapidxml::xml_node<> *step) {
          int linenum = getLinenum(step);
          #ifndef NDEBUG
          std::cout << "│ │ ├─Parsing step on line " << linenum << std::endl;
          #endif

          char* raw = getNode(step, "raw");
          char* rule = getNode(step, "rule");
          char* premise = getNode(step, "premise");
          #ifndef NDEBUG
          if (raw) std::cout << "│ │ │ ├─raw: \"" << raw << "\"" << std::endl;
          std::cout << "│ │ │ ├─rule: \"" << rule << "\"" << std::endl;
          if (premise) std::cout << "│ │ │ ├─premise: \"" << premise << "\"" << std::endl;
          #endif

          if (raw) parseRaw(raw);
        }

        /**
         * Parses a goal
         * 
         * @param goal the goal to parse
         *
         * @return the proof
         */
        void parseGoal(rapidxml::xml_node<> *goal) {
          #ifndef NDEBUG
          std::cout << "│ │ ├─Parsing goal" << std::endl;
          #endif

          char* raw = getNode(goal, "raw");
          #ifndef NDEBUG
          std::cout << "│ │ │ ├─raw: \"" << raw << "\"" << std::endl;
          #endif

          parseRaw(raw);

          #ifndef NDEBUG
          std::cout << "│ │ │ ╰─Goal parsed" << std::endl;
          #endif
        }

        /**
         * Parses a proof
         * 
         * @param proof the proof to parse
         *
         * @return the proof
         */
        void parseProof(rapidxml::xml_node<> *proof, std::unordered_map<int, void*> &proofMap) {
          int id = atoiNullptr(getAttribute(proof, "id"));

          #ifndef NDEBUG
          std::cout << "├─Parsing proof " << id << std::endl;
          #endif

          #ifndef NDEBUG
          std::cout << "│ ├─Parsing assumptions" << std::endl;
          #endif
          for (rapidxml::xml_node<> *assumption = proof->first_node("assumption"); assumption; assumption = assumption->next_sibling("assumption"))
            parseAssumption(assumption);

          #ifndef NDEBUG
          std::cout << "│ ├─Parsing steps"<< std::endl;
          #endif
          for (rapidxml::xml_node<> *step = proof->first_node("step"); step; step = step->next_sibling("step"))
            parseStep(step);

          #ifndef NDEBUG
          std::cout << "│ ├─Parsing goals"<< std::endl;
          #endif
          for (rapidxml::xml_node<> *goal = proof->first_node("goal"); goal; goal = goal->next_sibling("goal"))
            parseGoal(goal);

          proofMap[id] = nullptr;
        }
      };
    }

    void *parse(const char* file) {
      #ifndef NDEBUG
    std::cout << "Copying file for safety:" << std::endl;
    #endif

    std::size_t len = strlen(file);
    #ifndef NDEBUG
    std::cout << "|-File length: " << len << " bytes" << std::endl;
    #endif
    char *text = strdup(file);
    #ifndef NDEBUG
    std::cout << "╰─Coppied: " << strlen(text) << " bytes" << std::endl;
    #endif


    #ifndef NDEBUG
    std::cout << "Parsing XML: ";
    #endif
    rapidxml::xml_document<> doc;
    doc.parse<0>(text);
    rapidxml::xml_node<> *bram = doc.first_node("bram");
    #ifndef NDEBUG
    std::cout << "Done" << std::endl;
    #endif
    if (!bram) {
      #ifndef NDEBUG
      std::cout << "ERROR: bram node not found" << std::endl;
      #endif
      return nullptr;
    }


    #ifndef NDEBUG
    std::cout << "Getting metadata" << std::endl;
    #endif
    const char* program = getNode(bram, "program");
    #ifndef NDEBUG
    std::cout << "├─Program: " << program << std::endl;
    #endif
    const char* version = getNode(bram, "version");
    #ifndef NDEBUG
    std::cout << "├─Version: " << version << std::endl;
    #endif
    const char* author = getNode(bram->first_node("metadata"), "author");
    #ifndef NDEBUG
    std::cout << "╰─Author: " << author << std::endl;
    #endif



    #ifndef NDEBUG
    std::cout << "Parsing proofs:" << std::endl;
    #endif
    std::unordered_map<int, void*> proofMap;
    for (rapidxml::xml_node<> *proof = bram->first_node("proof"); proof; proof = proof->next_sibling("proof")) {
      parseProof(proof, proofMap);
    }

    return nullptr;
    }
  }
}
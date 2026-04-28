#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tree_sitter/api.h>
#include <map>

#include "ast.hpp"
#include "bram.hpp"
#include "parser.hpp"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "utils/logger.hpp"

namespace lb {
  namespace Parser {
    namespace {
      extern "C" {
        TSLanguage *tree_sitter_bram(void);
      }
      const TSLanguage *tsBramLanguage = tree_sitter_bram();

      // Helper functions

      /// Gets the value of a child node with certain name
      /// 
      /// @param node the node to search in
      /// @param name the name of the node to search for
      /// 
      /// @return the value of the child node if found or nullptr if not found
      char* getNode(rapidxml::xml_node<> *node, const char* name) {
        return node->first_node(name)==nullptr ? nullptr : node->first_node(name)->value();
      }

      /// Gets the value of an attribute with certain name
      /// 
      /// @param node the node to search in
      /// @param name the name of the attribute to search for
      /// 
      /// @return the value of the attribute if found or nullptr if not found
      char* getAttribute(rapidxml::xml_node<> *node, const char* name) {
        return node->first_attribute(name)==nullptr ? nullptr : node->first_attribute(name)->value();
      }

      /// Converts a string to an integer
      /// 
      /// @param str the string to convert
      /// 
      /// @return the value as an integer or -1 if null string
      int atoiNullptr(const char* str) {
        return str==nullptr ? -1 : atoi(str);
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
      Expression *parseRaw(const char* raw) {
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing raw \"%s\"", raw);

        if (strlen(raw) == 0) {
          Utils::Logger::log(Utils::Logger::Level::WARNING, "Empty raw");
          return nullptr;
        }

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Stripping comments");
        std::stringstream rawStream(raw);
        std::string line;
        std::string trimmed = "";
        while (getline(rawStream, line)) {
          trimmed += strtok(line.data(), ";");
        }
        
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Creating tree-sitter parser");
        TSParser *parser = ts_parser_new();
        if (!ts_parser_set_language(parser, tsBramLanguage)) {
          Utils::Logger::log(Utils::Logger::Level::ERROR, "Language version mismatch");
          ts_parser_delete(parser);
          return nullptr;
        }
        ts_parser_set_logger(parser, Utils::Logger::tslogger);

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing \"%s\"", trimmed.c_str());
        trimmed += '\n';
        TSTree *ast = ts_parser_parse_string(parser, NULL, trimmed.c_str(), strlen(trimmed.c_str()));
        Utils::Logger::log(Utils::Logger::Level::TRACE, "AST created");
        if (ast == NULL) {
          Utils::Logger::log(Utils::Logger::Level::ERROR, "Parsing failed");
          ts_parser_delete(parser);
          return nullptr;
        }
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Cleaning tree-sitter parser");
        ts_parser_delete(parser);

        TSNode root = ts_tree_root_node(ast);
        const char* tmpstr = ts_node_string(root);
        Utils::Logger::log(Utils::Logger::Level::INFO, "AST: \"%s\"", tmpstr);
        free((void*)tmpstr);

        if (ts_node_has_error(root)) {
          Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed raw");
          ts_tree_delete(ast);
          return nullptr;
        }

        Expression *e = parseAST(root, raw);
        
        Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed: \"%s\"", e==nullptr?"NULL":e->data().data());

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing raw done");
        ts_tree_delete(ast);
        return e;
      }

      /// Parses an assumption
      /// 
      /// @param assumption the assumption to parse
      /// @param proof the proof to store the assumption in
      /// 
      /// @return the proof
      void parseAssumption(rapidxml::xml_node<> *assumption, Proof *proof) {
        int linenum = getLinenum(assumption);
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing assumption on line %d", linenum);

        char* raw = getNode(assumption, "raw");
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Raw: %s", raw);

        proof->assumptions[linenum] = new Assumption(raw, parseRaw(raw));

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing assumption on line %d done", linenum);
      }

      /// Parses a step
      /// 
      /// @param step the step to parse
      /// @param proof the proof to store the step in
      /// 
      /// @return the proof
      void parseStep(rapidxml::xml_node<> *step, Proof *proof) {
        int linenum = getLinenum(step);
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing step on line %d", linenum);

        char* raw = getNode(step, "raw");
        if (raw!=nullptr)
          Utils::Logger::log(Utils::Logger::Level::TRACE, "Raw: %s", raw);

        char* rulestr = getNode(step, "rule");
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Rule: %s", rulestr);
        Step::Rule rule;
        if (!strcmp(rulestr, "SUBPROOF"))
          rule = Step::Rule::SUBPROOF;
        else 
          rule = Step::Rule::UNKNOWN;

        char* premise = getNode(step, "premise");
        if (premise!=nullptr)
          Utils::Logger::log(Utils::Logger::Level::TRACE, "Premise: %s", premise);

        Step *s = new Step(raw==nullptr?"":raw, raw==nullptr?nullptr:parseRaw(raw), rule);
        for (rapidxml::xml_node<> *assumption = step->first_node("premise"); assumption; assumption = assumption->next_sibling("premise")) {
          s->premise.push_back(atoi(assumption->value()));
        }
        proof->steps[linenum] = s;

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing step on line %d done", linenum);
      }

      /// Parses a goal
      /// 
      /// @param goal the goal to parse
      /// @param proof the proof to store the goal in
      /// 
      /// @return the proof
      void parseGoal(rapidxml::xml_node<> *goal, Proof *proof) {
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing goal");

        char* raw = getNode(goal, "raw");
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Raw: %s", raw);

        proof->goals.push_back(new Goal(raw, parseRaw(raw)));

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing goal done");
      }

      /// Parses a proof
      /// 
      /// @param proof the proof to parse
      /// 
      /// @return the proof
      void parseProof(rapidxml::xml_node<> *proof, std::map<int, Proof*> &proofMap) {
        int id = atoiNullptr(getAttribute(proof, "id"));
        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing proof %d", id);

        Proof *p = new Proof();

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing assumptions");
        for (rapidxml::xml_node<> *assumption = proof->first_node("assumption"); assumption!=nullptr; assumption = assumption->next_sibling("assumption"))
          parseAssumption(assumption, p);
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing assumptions done");

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing steps");
        for (rapidxml::xml_node<> *step = proof->first_node("step"); step!=nullptr; step = step->next_sibling("step"))
          parseStep(step, p);
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing steps done");

        Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing goals");
        for (rapidxml::xml_node<> *goal = proof->first_node("goal"); goal!=nullptr; goal = goal->next_sibling("goal"))
          parseGoal(goal, p);
        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing goals done");

        proofMap[id] = p;

        Utils::Logger::log(Utils::Logger::Level::TRACE, "Parsing proof %d done", id);
      }
    }

    
    BramFile *parse(const std::string &contents) {
      Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing contents");

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Copying contents for memory safety");

      Utils::Logger::log(Utils::Logger::Level::TRACE, "Buffer length: %lu chars", contents.length());
      std::string *text = new std::string(contents);
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Coppied: %lu chars", text->length());


      Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing XML");
      rapidxml::xml_document<> doc;
      doc.parse<0>(text->data());
      rapidxml::xml_node<> *bram = doc.first_node("bram");
      if (bram==nullptr) {
        Utils::Logger::log(Utils::Logger::Level::ERROR, "Malformed XML: bram node not found");
        delete text;
        return nullptr;
      }

      BramFile *bramfile = new BramFile;
      Utils::Logger::log(Utils::Logger::Level::INFO, "Getting metadata");
      const char* program = getNode(bram, "program");
      if (program==nullptr)
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: program node not found");
      if (strlen(program) == 0) {
        program = nullptr;
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: program empty");
      }
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Program: %s", program);
      bramfile->program = program==nullptr?"UNKOWN":program;
      const char* version = getNode(bram, "version");
      if (version==nullptr)
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: version node not found");
      if (strlen(version) == 0) {
        version = nullptr;
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: version empty");
      }
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Version: %s", version);
      bramfile->version = version==nullptr?"UNKOWN":version;
      const char* author = getNode(bram->first_node("metadata"), "author");
      if (author==nullptr)
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: author node not found");
      if (strlen(author) == 0) {
        author = nullptr;
        Utils::Logger::log(Utils::Logger::Level::WARNING, "Malformed XML: author empty");
      }
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Author: %s", author);
      bramfile->author = author==nullptr?"UNKOWN":author;

      Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing proofs");

      for (rapidxml::xml_node<> *proof = bram->first_node("proof"); proof!=nullptr; proof = proof->next_sibling("proof")) {
        parseProof(proof, bramfile->proofs);
      }
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing proofs done");

      Utils::Logger::log(Utils::Logger::Level::INFO, "Parsing done");
      return bramfile;
    }

    BramFile *parseFile(const std::string &file, bool log) {
      if (log) {
        std::ofstream out(std::string(file) + ".log", std::ios::out | std::ios::ate);
        // Utils::Logger::setOutputStream(out);
      }
      rapidxml::file<> xmlFile(file.data());
      BramFile *bramfile = parse(xmlFile.data());
      if (log) Utils::Logger::setOutputStream();
      return bramfile;
    }
  }
}

#include "parser.hpp"
#include "utils/logger.hpp"
#include <iostream>

void printProof(lb::BramFile *bf, lb::Proof *proof, int indent=0) {
  for (auto it : proof->assumptions)
    std::cout << std::string(indent+1, '|') << " " << it.first+1 << " \t" << it.second->raw << std::endl;

  std::cout << std::string(indent, '|') << "|-----" << std::endl;

  for (auto it : proof->steps) {
    if (it.second->rule == lb::Step::SUBPROOF) {
      int subprfnum = it.second->premise[0];
      printProof(bf, bf->proofs[subprfnum], indent+1);
    } else {
      std::cout << std::string(indent+1, '|') << " " << it.first+1 << " \t" << it.second->raw << " \t" << it.second->rule << " :";
      for (auto it2 : it.second->premise)
        std::cout << " " << it2+1;
      std::cout << std::endl;
    }
  }

  for (auto it : proof->goals)
    std::cout << "GOAL: " << it->raw << std::endl;
}

int main(int argc, char **argv) {
  try {
    if (argc == 1) {
      lb::BramFile *bf = lb::Parser::parseFile("tests/example.bram", true);
      std::cout << "Program: " << bf->program << std::endl;
      std::cout << "Version: " << bf->version << std::endl;
      std::cout << "Author: " << bf->author << std::endl;
      printProof(bf, bf->proofs[0]);
    }
    for (int i = 1; i < argc; i++) {
      std::cout << argv[i] << ":" << std::endl;
      lb::BramFile *bf = lb::Parser::parseFile(argv[i], true);
      std::cout << "Program: " << bf->program << std::endl;
      std::cout << "Version: " << bf->version << std::endl;
      std::cout << "Author: " << bf->author << std::endl;
      printProof(bf, bf->proofs[0]);
      std::cout << std::endl << std::endl;
    }
  } catch (std::exception& e) {
    lb::Utils::Logger::log(lb::Utils::Logger::Level::ERROR, e.what());
  }
}
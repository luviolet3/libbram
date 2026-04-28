#include "parser.hpp"
#include "utils/logger.hpp"
#include <iostream>

void printProof(lb::BramFile *bf, lb::Proof *proof, int indent=0) {
  for (auto it : proof->assumptions)
    std::cout << std::string("|", indent+1) << " " << it.first << "\t" << it.second->assumption->data() << std::endl;
  std::cout << std::string("|", indent) << "|-----" << std::endl;
  for (auto it : proof->steps)
    std::cout << std::string("|", indent+1) << " " << it.first << "\t" << (it.second->step==nullptr?"":it.second->step->data()) << "\t\t" << it.second->rule << std::endl;
}

int main(int argc, char **argv) {
  try {
    lb::BramFile *bf = lb::Parser::parseFile(argc==1?"tests/example.bram":argv[1], true);
    std::cout << "Program: " << bf->program << std::endl;
    std::cout << "Version: " << bf->version << std::endl;
    std::cout << "Author: " << bf->author << std::endl;

    printProof(bf, bf->proofs[0]);
  } catch (std::exception& e) {
    lb::Utils::Logger::log(lb::Utils::Logger::Level::ERROR, e.what());
  }
}
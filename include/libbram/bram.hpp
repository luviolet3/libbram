#ifndef LB_BRAM_HPP
#define LB_BRAM_HPP

#include <string>
#include <map>
#include <vector>

#include "ast.hpp"

namespace lb {
  class Assumption {
    public:
    std::string raw;
    Expression *assumption;

    Assumption() : assumption(nullptr) {}
    Assumption(std::string raw, Expression *assumption) : raw(raw), assumption(assumption) {}
    Assumption(const Assumption &o) : assumption(o.assumption) {}
    ~Assumption() { delete assumption; }
  };

  class Step {
    public:
    enum Rule {
      UNKNOWN,
      SUBPROOF
    };
    std::string raw;
    Expression *step;
    Rule rule;
    std::vector<int> premise;

    Step() : step(nullptr), rule(Rule::UNKNOWN) {}
    Step(std::string raw, Expression *step, Rule rule) : raw(raw), step(step), rule(rule) {}
    Step(const Step &o) : step(o.step), rule(o.rule), premise(o.premise) {}
    ~Step() { delete step; }
  };

  class Goal {
    public:
    std::string raw;
    Expression *goal;

    Goal() : goal(nullptr) {}
    Goal(std::string raw, Expression *goal) : raw(raw), goal(goal) {}
    Goal(const Goal &o) : goal(o.goal) {}
    ~Goal() { delete goal; }
  };

  class Proof {
    public:
    std::map<int, Assumption*> assumptions;
    std::map<int, Step*> steps;
    std::vector<Goal*> goals;

    Proof() {}
    ~Proof() {
      for (std::pair<int, Assumption*> assumption : assumptions) delete assumption.second;
      for (std::pair<int, Step*> step : steps) delete step.second;
      for (Goal *goal : goals) delete goal;
    }
  };

  class BramFile {
    public:
    std::string program, version, author;
    std::map<int, Proof*> proofs;

    BramFile(std::string program="UNKNOWN", std::string version="UNKNOWN", std::string author="UNKNOWN") : program(program), version(version), author(author) {}
    BramFile(const BramFile &o) { BramFile(o.program, o.version, o.author); }
   };
};

#endif

%module(package="libbram", docstring="Base .bram file class and related classes") bram
%include "std_string.i"
%include "std_map.i"
%include "std_vector.i"
%include "std_common.i"

%{
  #include "bram.hpp"
  #include "ast.hpp"
%}

%template(map_int_assumption) std::map<int, lb::Assumption*>;
%template(map_int_step) std::map<int, lb::Step*>;
%template(map_int_proof) std::map<int, lb::Proof*>;
%template(vec_int) std::vector<int>;
%template(vec_goal) std::vector<lb::Goal*>;

namespace lb {
  class Assumption {
    public:
    std::string raw;
    lb::Expression *assumption;

    Assumption(std::string raw, lb::Expression *assumption);
    ~Assumption();
  };

  class Step {
    public:
    enum Rule {
      UNKNOWN
    };
    std::string raw;
    lb::Expression *step;
    Rule rule;
    std::vector<int> premise;

    Step(std::string raw, lb::Expression *step, Rule rule);
    ~Step();
  };

  class Goal {
    public:
    std::string raw;
    lb::Expression *goal;

    Goal(std::string raw, lb::Expression *goal);
    ~Goal();
  };

  class Proof {
    public:
    std::map<int, Assumption*> assumptions;
    std::map<int, Step*> steps;
    std::vector<Goal*> goals;

    Proof();
    ~Proof();
  };
}
%{
  namespace swig {
    template <> struct traits<lb::Assumption> {
      typedef pointer_category category;
      static const char* type_name() { return "Assumption"; }
    };
    template <> struct traits<lb::Step> {
      typedef pointer_category category;
      static const char* type_name() { return "Step"; }
    };

    template <> struct traits<lb::Proof> {
      typedef pointer_category category;
      static const char* type_name() { return "Proof"; }
    };
  }
%}
namespace lb {
  class BramFile {
    public:
    std::string program, version, author;
    std::map<int, Proof*> proofs;
    BramFile(std::string program="UNKNOWN", std::string version="UNKNOWN", std::string author="UNKNOWN");
    ~BramFile();
  };
}
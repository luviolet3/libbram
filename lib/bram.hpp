#ifndef LB_BRAM_HPP
#define LB_BRAM_HPP

#include "lib/utils.hpp"
#include "lib/expr.hpp"

namespace lb {
   struct Step {
      Expr expr;
      Rule rule;
      Arr<size_t> premIds; //the premises referenced by the rule
   };
   struct Proof {
      size_t id;
      Arr<Expr*> assumptions;
      Arr<Step*> steps;
      Expr goal;
      
   };
   struct BramFile {
      Str program;
      Str version;
      struct {
         Str author;
         Str created;
         Str modified;
      } meta;

   };
};

#endif

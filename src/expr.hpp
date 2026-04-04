#ifndef LB_EXPR_HPP
#define LB_EXPR_HPP

#include "lib/utils.hpp"

namespace lb {
   struct Str {
      char* ptr;
      size_t size;
   };

   enum class Op : uint8_t {
      NULL = 0, //null operator

      CONJ,     //conjunction
      DISJ,     //disjunction
      LNOT,     //negation
      COND,     //conditional
      BICOND,   //biconditional
      EQUIV,    //equivelance
      NEQUIV,   //non-equivelance
      ADD,      //addition
      MULT,     //multiplication

      FORALL,   //for all
      EXISTS,   //there exists

      TAUT,     //tautology
      CONT,     //contradiction
   };
   uint8_t constexpr operator+(Op op) { return (uint8_t)op; }
   bool isAssoc(Op);

   enum class ExprTag : uint8_t {
      NULL = 0,

      LITERAL = 1 << 0, //literal expression
      UNARY   = 1 << 1, //unary operator (else binary)
      CALL    = 1 << 2, //function call syntax: EX: P(x): lhs = P, rhs = x
      ASSOC   = 1 << 3, //associative expression (EX: lhs CONJ rhs)
      QUANT   = 1 << 4, //quantifier expression (FORALL and EXISTS)
      NFORDER = 1 << 5, //not part of first-order logic
   };
   uint8_t constexpr operator+(ExprTag tag) { return (uint8_t)tag; }

   class Expr {
      private:
         union {
            struct {
               Expr* _lhs;
               Expr* _rhs;
            };
            Str _var;
         };
         Op _op; //_op == NULL -> variable
         ExprTag _tag;
         bool _swapSides;
         size_t _hash; //!TODO: implement hashing
      public:
         Expr(Op, Expr*, Expr*);
         Expr(Str);

         Expr* lhs();
         Expr* rhs();
         Expr const* lhs() const;
         Expr const* rhs() const;
         Op op() const { return _op; }
         ExprTag tag() const { return _tag; }
   };


   enum class Axiom : uint8_t { //also includes inference rules
      NULL = 0,

      //introduction rules
      INTRO_CONJ,
      INTRO_DISJ,
      INTRO_LNOT,
      INTRO_COND,
      INTRO_BICOND,
      INTRO_EQUIV,
      INTRO_NEQUIV,

      INTRO_FORALL,
      INTRO_EXISTS,

      INTRO_TAUT,
      INTRO_CONT,

      //elimination rules
      ELIM_CONJ,
      ELIM_DISJ,
      ELIM_LNOT,
      ELIM_COND,
      ELIM_BICOND,
      ELIM_EQUIV,
      ELIM_NEQUIV,

      ELIM_FORALL,
      ELIM_EXISTS,

      ELIM_TAUT,
      ELIM_CONT,

      //other rules
      IND_PEANO,  //Peano induction
      IND_STRONG, //strong induction
      REIT,       //reiteration
      SUBPROOF,   //creation of a subproof
      LEMMA,      //application of a lemma

      //helpers
      HELPER_INTRO_FIRST = INTRO_CONJ,
      HELPER_INTRO_LAST  = INTRO_CONT,
      HELPER_ELIM_FIRST  =  ELIM_CONJ,
      HELERP_ELIM_LAST   =  ELIM_CONT,
   };
   uint8_t constexpr operator+(Axiom axiom) { return (uint8_t)axiom; }
   bool constexpr isIntro(Axiom axiom) { return +axiom >= +Axiom::HELPER_INTRO_FIRST && +axiom <= +Axiom::HELPER_INTRO_LAST; }
   bool constexpr isElim (Axiom axiom) { return +axiom >= +Axiom::HELPER_ELIM_FIRST  && +axiom <= +Axiom::HELPER_ELIM_LAST;  }
   class Rule {
      private:
         Axiom _axiom;
         Proof* _lemma; //nonnull if and only if axiom == LEMMA
      public:
         Rule(Axiom axiom):
            _axiom(axiom), _lemma(nullptr) {}
         Rule(Proof* lemma):
            _axiom(Axiom::LEMMA), _lemma(lemma) {}

         Axiom axiom() const { return _axiom; }
         Proof* lemma() { return _lemma; }
         Proof const* lemma() const { return _lemma; }
   };
};

#endif

#include "expr.hpp"

//array mapping operators to their corresponding expression tags
constexpr lb::ExprTag tagMap[] = {
   [+lb::Op::null  ] = lb::ExprTag::null,

   [+lb::Op::CONJ  ] = lb::ExprTag::ASSOC,
   [+lb::Op::DISJ  ] = lb::ExprTag::ASSOC,

   [+lb::Op::LNOT  ] = lb::ExprTag::UNARY,

   [+lb::Op::COND  ] = lb::ExprTag::null,
   [+lb::Op::BICOND] = lb::ExprTag::ASSOC,
   [+lb::Op::EQUIV ] = lb::ExprTag::ASSOC,
   [+lb::Op::NEQUIV] = lb::ExprTag::ASSOC,

   [+lb::Op::ADD   ] = lb::ExprTag::ASSOC | lb::ExprTag::NFORDER,
   [+lb::Op::MULT  ] = lb::ExprTag::ASSOC | lb::ExprTag::NFORDER,

   [+lb::Op::FORALL] = lb::ExprTag::QUANT,
   [+lb::Op::EXISTS] = lb::ExprTag::QUANT,

   [+lb::Op::TAUT  ] = lb::ExprTag::LITERAL,
   [+lb::Op::CONT  ] = lb::ExprTag::LITERAL
};

bool lb::isAssoc(Op op) {
   return +tagMap[+op] & +ExprTag::ASSOC;
}

lb::Expr::Expr(Op op, Expr* lhs, Expr* rhs):
   _lhs(lhs),
   _rhs(rhs),
   _op(op),
   _tag(tagMap[+op]) {
      size_t hash;
      //handle literals
      if (+_tag & +ExprTag::LITERAL) {
         if (lhs || rhs) {
            TODO;
         }
         hash = +op;
      }
      //handle unary operators
      else if (+_tag & +ExprTag::UNARY) {
         if (!lhs || rhs) {
            TODO;
         }
      }
      //handle vinary operators
      else {
         if (!lhs || !rhs) {
            TODO;
         }
      }
      _hash = hash;
}

lb::Expr::Expr(char* varName):
   _var(varName),
   _op(Op::null),
   _tag(ExprTag::LITERAL) {

}

lb::Expr* lb::Expr::lhs() {
   if (_op == Op::null) {
      return nullptr;
   }
   return _lhs;
}
lb::Expr* lb::Expr::rhs() {
   if (_op == Op::null) {
      return nullptr;
   }
   return _rhs;
}
lb::Expr const* lb::Expr::lhs() const {
   if (_op == Op::null) {
      return nullptr;
   }
   return _lhs;
}
lb::Expr const* lb::Expr::rhs() const {
   if (_op == Op::null) {
      return nullptr;
   }
   return _rhs;
}

#include "expr.cpp"

//array mapping operators to their corresponding expression tags
constexpr lb::ExprTag tagMap[] = {
   [+lb::Op::NULL  ] = lb::ExprTag::NULL,

   [+lb::Op::CONJ  ] = lb::ExprTag::ASSOC,
   [+lb::Op::DISJ  ] = lb::ExprTag::ASSOC,

   [+lb::Op::LNOT  ] = lb::ExprTag::UNARY,

   [+lb::Op::COND  ] = lb::ExprTag::NULL,
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
   
}

lb::Expr::Expr(Str varName):
   _var(varName),
   _op(Op::NULL),
   _tag(ExprTag::LITERAL) {

}

lb::Expr* lb::Expr::lhs() {
   if (_op == Op::NULL) {
      return nullptr;
   }
   return _lhs;
}
lb::Expr* lb::Expr::rhs() {
   if (_op == Op::NULL) {
      return nullptr;
   }
   return _rhs;
}
lb::Expr const* lb::Expr::lhs() const {
   if (_op == Op::NULL) {
      return nullptr;
   }
   return _lhs;
}
lb::Expr const* lb::Expr::rhs() const {
   if (_op == Op::NULL) {
      return nullptr;
   }
   return _rhs;
}

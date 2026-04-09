#ifndef LB_AST_EXPRESSION_HPP
#define LB_AST_EXPRESSION_HPP

#include <cpp-tree-sitter.h>
#include <cstring>

namespace lb {
  enum class Op {
    CONJ,     // onjunction
    DISJ,     // disjunction
    COND,     // conditional
    BICOND,   // biconditional
    EQUIV,    // equivelance
    ADD,     // addition
    MULT,     // multiplication

    FORALL,   // for all
    EXISTS,   // there exists

    NEG,      // negation

    VAR,      // variable
    TAUT,     // tautology
    CONT,     // contradiction
  };

  /**
   * The root for an AST
   */
  class Expression {
    public:
    virtual ~Expression() {}
    virtual char *data() = 0;
  };

  /**
   * A binary expression has 2 expressions joined by an operator
   */
  class BinaryExpression : public Expression {
    private:
    Op op;
    Expression *lhs, *rhs;
    public:
    BinaryExpression(Op op, Expression *lhs, Expression *rhs) : op(op), lhs(lhs), rhs(rhs) {};
    ~BinaryExpression() {
      if (lhs != nullptr) delete lhs;
      if (rhs != nullptr) delete rhs;
    }
    char *data();
  };

  /**
   * An unary expression has 1 expression and an operator
   */
  class UnaryExpression : public Expression {
    private:
    Op op;
    Expression *inner;
    public:
    UnaryExpression(Op op, Expression *inner) : op(op), inner(inner) {};
    ~UnaryExpression() { if (inner != nullptr) delete inner; }
    char *data();
  };

/**
   * A miscellaneous expression has an operator and some other data
   */
  class MiscExpression : public Expression {
    private:
    Op op;
    const char *inner;
    public:
    MiscExpression(Op op, const char *data) : op(op), inner(data) {};
    ~MiscExpression() { if (inner != nullptr) delete inner; }
    char *data() { return strdup(inner); }
  };

  class Conjunction : public BinaryExpression { public: Conjunction(Expression *lhs, Expression *rhs) : BinaryExpression(Op::CONJ, lhs, rhs) {} };
  class Disjunction : public BinaryExpression { public: Disjunction(Expression *lhs, Expression *rhs) : BinaryExpression(Op::DISJ, lhs, rhs) {} };
  class Conditional : public BinaryExpression { public: Conditional(Expression *lhs, Expression *rhs) : BinaryExpression(Op::COND, lhs, rhs) {} };
  class Biconditional : public BinaryExpression { public: Biconditional(Expression *lhs, Expression *rhs) : BinaryExpression(Op::BICOND, lhs, rhs) {} };
  class Equivelence : public BinaryExpression { public: Equivelence(Expression *lhs, Expression *rhs) : BinaryExpression(Op::EQUIV, lhs, rhs) {} };
  class Addition : public BinaryExpression { public: Addition(Expression *lhs, Expression *rhs) : BinaryExpression(Op::ADD, lhs, rhs) {} };
  class Multiplication : public BinaryExpression { public: Multiplication(Expression *lhs, Expression *rhs) : BinaryExpression(Op::MULT, lhs, rhs) {} };
  class Forall : public BinaryExpression { public: Forall(Expression *lhs, Expression *rhs) : BinaryExpression(Op::FORALL, lhs, rhs) {} };
  class Exists : public BinaryExpression { public: Exists(Expression *lhs, Expression *rhs) : BinaryExpression(Op::EXISTS, lhs, rhs) {} };
  class Negation : public UnaryExpression { public: Negation(Expression *inner) : UnaryExpression(Op::NEG, inner) {} };
  class Variable : public MiscExpression { public: Variable(const char *name) : MiscExpression(Op::VAR, name) {} };
  class Tautology : public MiscExpression { public: Tautology() : MiscExpression(Op::VAR, "⊤") {} };
  class Contradiction : public MiscExpression { public: Contradiction() : MiscExpression(Op::VAR, "⊥") {} };

  Expression *parseAST(ts::Node root, const char* source);
}

#endif
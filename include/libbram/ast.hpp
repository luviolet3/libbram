#ifndef LB_AST_EXPRESSION_HPP
#define LB_AST_EXPRESSION_HPP

#include <cstdarg>
#include <cstdlib>
#include <string.h>
#include <tree_sitter/api.h>

namespace lb {
  enum class Op {
    UNKNOWN=-1,

    CONJ,     // conjunction
    DISJ,     // disjunction
    COND,     // conditional
    BICOND,   // biconditional
    EQUIV,    // equivelance
    ADD,      // addition
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
    virtual char* data() = 0;
  };

  /**
   * A binary expression has many expressions joined by an operator
   */
  class VariableExpression : public Expression {
    public:
    Op op;
    Expression **expressions;

    VariableExpression(Op op, Expression **expressions) : op(op), expressions(expressions) {};
    VariableExpression(Op op, unsigned int count, ...) {
      va_list args;
      va_start(args, count);

      expressions = (Expression**)calloc(count, sizeof(Expression*));

      for (int i = 0; i < count; i++)
        expressions[i] = va_arg(args, Expression*);

      va_end(args);
    };
    ~VariableExpression() {
      free(expressions);
    }
    char* data();
  };

  /**
   * A binary expression has 2 expressions joined by an operator
   */
  class BinaryExpression : public Expression {
    public:
    Op op;
    Expression *lhs, *rhs;

    BinaryExpression(Op op, Expression *lhs, Expression *rhs) : op(op), lhs(lhs), rhs(rhs) {};
    ~BinaryExpression() {
      if (lhs != nullptr) delete lhs;
      if (rhs != nullptr) delete rhs;
    }
    char* data();
  };

  /**
   * An unary expression has 1 expression and an operator
   */
  class UnaryExpression : public Expression {
    public:
    Op op;
    Expression *inner;

    UnaryExpression(Op op, Expression *inner) : op(op), inner(inner) {};
    ~UnaryExpression() { if (inner != nullptr) delete inner; }
    char* data();
  };

/**
   * A miscellaneous expression has an operator and some other data
   */
  class MiscExpression : public Expression {
    public:
    Op op;
    const char *inner;

    MiscExpression(Op op, const char *data) : op(op), inner(data) {};
    ~MiscExpression() { if (inner != nullptr) free((void*)inner); }
    char* data() { return strdup(inner); }
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

  Expression *parseAST(TSNode root, const char* source);
}

#endif

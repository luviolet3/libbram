#include "ast.hpp"
#include "utils/logger.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

namespace lb {
  namespace {
    extern "C" {
      TSLanguage *tree_sitter_bram(void);
    }
    const ts::Language tsBramLanguage = tree_sitter_bram();

    const char* opString(Op op) {
      switch (op) {
        case Op::CONJ: return "∧";
        case Op::DISJ: return "∨";
        case Op::COND: return "→";
        case Op::BICOND: return "↔";
        case Op::EQUIV: return "≡";
        case Op::ADD: return "+";
        case Op::MULT: return "*";
        case Op::FORALL: return "∀";
        case Op::EXISTS: return "∃";
        case Op::NEG: return "¬";
        case Op::VAR: return "";
        case Op::TAUT: return "⊤";
        case Op::CONT: return "⊥";
        default: return "(UNKNOWN OP)";
      }
    }

    Expression *parseAssoc(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing assoc_term");
      Utils::Logger::indent();

      Expression *e = nullptr;
      

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed assoc_term: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }

    Expression *parseExpr(ts::Node expr, const char* source);
    Expression *parseImpl(ts::Node expr, const char* source);
    Expression *parseParen(ts::Node expr, const char* source);
    Expression *parsePred(ts::Node expr, const char* source);
    Expression *parseNot(ts::Node expr, const char* source);
    Expression *parseBinder(ts::Node expr, const char* source);
    Expression *parseVariable(ts::Node expr, const char* source);
    // Expression *parseExpr(ts::Node expr, const char* source);
    // Expression *parseExpr(ts::Node expr, const char* source);
    // Expression *parseExpr(ts::Node expr, const char* source);

    Expression *parseVar(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing variable");
      Utils::Logger::indent();

      unsigned int start = expr.getByteRange().start;
      unsigned int end = expr.getByteRange().end;
      unsigned int len = end - start;
      Utils::Logger::log(Utils::Logger::Level::TRACE, "%s[%d:%d]", source, start, end);
      Expression *e = new Variable(strndup(source + start, len));

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed variable: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }

    Expression *parsePred(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing predicate");
      Utils::Logger::indent();

      Expression *e = parseVar(expr.getChild(0), source);
      if (expr.getNumChildren() == 2) {
        ts::Node args = expr.getChildByFieldName("arguments");
        e = nullptr;
        Utils::Logger::log(Utils::Logger::Level::ERROR, "TODO: parsePred");
      }

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed predicate: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }

    Expression *parseNot(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing notterm");
      Utils::Logger::indent();

      Expression *e = new Negation(parseParen(expr.getChild(0), source));

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed notterm: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }

    Expression *parseBinder(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing binder");
      Utils::Logger::indent();

      Expression *e = nullptr;
      ts::Node inner = expr.getChild(2);
      if (inner.getType() == "expr") {
        e = parseExpr(inner, source);
      } else if (inner.getType() == "paren_expr") {
        e = parseParen(inner, source);
      }
      if (e != nullptr) {
        Expression *var = parseVar(expr.getChild(1), source);
        ts::Node quant = expr.getChild(0).getChild(0);
        if (quant.getType() == "forall_quantifier") {
          e = new Forall(var, e);
        } else if (quant.getType() == "exists_quantifier") {
          e = new Exists(var, e);
        }
      }

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed binder: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }

    Expression *parseParen(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing paren_expr");
      Utils::Logger::indent();

      Expression *e = nullptr;
      ts::Node child = expr.getChild(0);
      if (child.getType() == "contradiction") {
        e = new Contradiction();
      } else if (child.getType() == "tautology") {
        e = new Tautology();
      } else if (child.getType() == "predicate") {
        e = parsePred(child, source);
      } else if (child.getType() == "notterm") {
        e = parseNot(child, source);
      } else if (child.getType() == "binder") {
        e = parseBinder(child, source);
      } else if (child.getType() == "expr") {
        e = parseExpr(child, source);
      }

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed paren_expr: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }

    Expression *parseImpl(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing impl_term");
      Utils::Logger::indent();

      Expression *lhs = parseParen(expr.getChild(0), source);
      Expression *rhs = parseParen(expr.getChild(0), source);
      Expression *e = new Conditional(lhs, rhs);

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed impl_term: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }

    Expression *parseExpr(ts::Node expr, const char* source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing expr");
      Utils::Logger::indent();

      Expression *e = nullptr;
      ts::Node child = expr.getChild(0);
      if (child.getType() == "assoc_term") {
        e = parseAssoc(child, source);
      } else if (child.getType() == "impl_term") {
        e = parseImpl(child, source); 
      } else if (child.getType() == "paren_expr") {
        e = parseParen(child, source);
      }

      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed expr: %s", e==nullptr?"NULL":e->data());
      Utils::Logger::unindent();
      Utils::Logger::flush();
      return e;
    }
  }

  char *BinaryExpression::data() {
    const char* l = lhs==nullptr?"NULL":lhs->data();
    const char* r = lhs==nullptr?"NULL":rhs->data();
    const char* o = opString(op);
    char* buf = (char*)calloc(strlen(l)+strlen(o+strlen(r))+6, sizeof(char));
    sprintf(buf, "(%s %s %s)", l, o, r);
    // if (l != nullptr) delete l;
    // if (r != nullptr) delete r;
    return buf;
  }
  char *UnaryExpression::data() {
    const char* i = inner==nullptr?"NULL":inner->data();
    const char* o = opString(op);
    char* buf = (char*)calloc(strlen(o+strlen(i))+2, sizeof(char));
    sprintf(buf, "%s%s", o, i);
    // if (i != nullptr) delete i;
    return buf;
  }
  
  Expression *parseAST(ts::Node root, const char* source) { return parseExpr(root.getChild(0), source); }
}
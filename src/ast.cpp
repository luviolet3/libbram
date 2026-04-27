#include "ast.hpp"
#include "utils/logger.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#define LOG_PARSED(name) do {\
  const char* tmpstr = e ? e->data() : "NULL"; \
  Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsed " name ": %s", tmpstr); \
  if (e) { free((void*)tmpstr); } \
} while (0)

namespace lb {
  namespace {
    extern "C" {
      TSLanguage *tree_sitter_bram(void);
    }
    const TSLanguage *tsBramLanguage = tree_sitter_bram();

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

    Expression *parseExpr(TSNode expr, const char *source);
    Expression *parseImpl(TSNode expr, const char *source);
    Expression *parseParen(TSNode expr, const char *source);
    Expression *parsePred(TSNode expr, const char *source);
    Expression *parseNot(TSNode expr, const char *source);
    Expression *parseBinder(TSNode expr, const char *source);
    Expression *parseVar(TSNode expr, const char *source);

    Expression *parseAssoc(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing assoc_term with %s", source);


      Op op;
      const char *opstr = ts_node_type(ts_node_child(expr, 1));
      if (!strcmp(opstr, "∧")) op = Op::CONJ;
      else if (!strcmp(opstr, "∨")) op = Op::DISJ;
      else if (!strcmp(opstr, "↔")) op = Op::BICOND;
      else if (!strcmp(opstr, "≡")) op = Op::EQUIV;
      else if (!strcmp(opstr, "+")) op = Op::ADD;
      else if (!strcmp(opstr, "*")) op = Op::MULT;
      else op = Op::UNKNOWN;
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Op: %d", op);

      Expression **expressions = (Expression**)calloc(ts_node_child_count(expr) / 2 + 2, sizeof(Expression*));
      for (int i = 0; i <= ts_node_child_count(expr) / 2; i ++)
        expressions[i] = parseParen(ts_node_child(expr, i * 2), source);
      Expression *e = new VariableExpression(op, expressions);

      LOG_PARSED("assoc_term");


      return e;
    }

    Expression *parseVar(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing variable with %s", source);


      unsigned int start = ts_node_start_point(expr).column;
      unsigned int end = ts_node_end_point(expr).column;
      unsigned int len = end - start;
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Range: [%d - %d] (length %d)", start, end, len);
      const char* str = strndup(source + start, len);
      Utils::Logger::log(Utils::Logger::Level::TRACE, "Name: %s", str);
      Expression *e = new Variable(str);

      LOG_PARSED("variable");


      return e;
    }

    Expression *parsePred(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing predicate with %s", source);


      Expression *e = parseVar(ts_node_child(expr, 0), source);
      if (ts_node_child_count(expr) == 2) {
        TSNode args = ts_node_child_by_field_name(expr, "arguments", strlen("arguments"));
        Expression **expressions = (Expression**)calloc(ts_node_child_count(args) / 2 + 1, sizeof(Expression*));
        for (int i = 0; i <= ts_node_child_count(expr) / 2; i ++)
          expressions[i] = parseParen(ts_node_child(args, i * 2), source);
        Expression *e = new VariableExpression(Op::UNKNOWN, expressions);
      }

      LOG_PARSED("predicate");


      return e;
    }

    Expression *parseNot(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing notterm with %s", source);


      Expression *e = new Negation(parseParen(ts_node_child(expr, 0), source));

      LOG_PARSED("notterm");


      return e;
    }

    Expression *parseBinder(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing binder with %s", source);


      Expression *e = nullptr;
      TSNode inner = ts_node_child(expr, 2);
      if (!strcmp(ts_node_type(inner), "expr")) {
        e = parseExpr(inner, source);
      } else if (!strcmp(ts_node_type(inner), "paren_expr")) {
        e = parseParen(inner, source);
      }
      if (e != nullptr) {
        Expression *var = parseVar(ts_node_child(expr, 1), source);
        TSNode quant = ts_node_child(ts_node_child(expr, 0), 0);
        if (!strcmp(ts_node_type(quant), "forall_quantifier")) {
          e = new Forall(var, e);
        } else if (!strcmp(ts_node_type(quant), "exists_quantifier")) {
          e = new Exists(var, e);
        }
      }

      LOG_PARSED("binder");


      return e;
    }

    Expression *parseParen(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing paren_expr with %s", source);


      Expression *e = nullptr;
      TSNode child = ts_node_child(expr, 0);
      if (!strcmp(ts_node_type(child), "contradiction")) {
        e = new Contradiction();
      } else if (!strcmp(ts_node_type(child), "tautology")) {
        e = new Tautology();
      } else if (!strcmp(ts_node_type(child), "predicate")) {
        e = parsePred(child, source);
      } else if (!strcmp(ts_node_type(child), "notterm")) {
        e = parseNot(child, source);
      } else if (!strcmp(ts_node_type(child), "binder")) {
        e = parseBinder(child, source);
      } else if (!strcmp(ts_node_type(child), "expr")) {
        e = parseExpr(child, source);
      }

      LOG_PARSED("paren_expr");


      return e;
    }

    Expression *parseImpl(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing impl_term with %s", source);


      Expression *lhs = parseParen(ts_node_child(expr, 0), source);
      Expression *rhs = parseParen(ts_node_child(expr, 2), source);
      Expression *e = new Conditional(lhs, rhs);

      LOG_PARSED("impl_term");


      return e;
    }

    Expression *parseExpr(TSNode expr, const char *source) {
      Utils::Logger::log(Utils::Logger::Level::DEBUG, "Parsing expr with %s", source);


      Expression *e = nullptr;
      TSNode child = ts_node_child(expr, 0);
      if (!strcmp(ts_node_type(child), "assoc_term")) {
        e = parseAssoc(child, source);
      } else if (!strcmp(ts_node_type(child), "impl_term")) {
        e = parseImpl(child, source); 
      } else if (!strcmp(ts_node_type(child), "paren_expr")) {
        e = parseParen(child, source);
      }

      LOG_PARSED("expr");


      return e;
    }
  }

 char *VariableExpression::data() {
    std::string buf = "(";
    for (int i = 0; expressions[i] != nullptr; i++) {
      if (i) {
        buf += " ";
        buf += opString(op);
        buf += " ";
      }
      char* tmp = expressions[i]->data();
      buf += tmp;
      free(tmp);
    }
    buf += ")";
    return strdup(buf.data());
  }
  char *BinaryExpression::data() {
    const char *l = lhs==nullptr?"NULL":lhs->data();
    const char *r = lhs==nullptr?"NULL":rhs->data();
    const char *o = opString(op);
    char *buf = (char*)calloc(strlen(l)+strlen(o)+strlen(r)+5, sizeof(char));
    sprintf(buf, "(%s %s %s)", l, o, r);
    if (lhs != nullptr) free((void*)l);
    if (rhs != nullptr) free((void*)r);
    return buf;
  }
  char *UnaryExpression::data() {
    const char *i = inner==nullptr?"NULL":inner->data();
    const char *o = opString(op);
    char *buf = (char*)calloc(strlen(o)+strlen(i)+1, sizeof(char));
    sprintf(buf, "%s%s", o, i);
    if (inner != nullptr) free((void*)i);
    return buf;
  }
  
  Expression *parseAST(TSNode root, const char *source) { return parseExpr(ts_node_child(root, 0), source); }
}

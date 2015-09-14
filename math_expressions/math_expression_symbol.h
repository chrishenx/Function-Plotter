#ifndef MATH_EXPRESSION_SYMBOL_H
#define MATH_EXPRESSION_SYMBOL_H

#include <string>
#include <ostream>

namespace math_expression {
  struct TerminalSymbol;
  struct Symbol;
  enum NonTerminalSymbol {
    A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N,
    O, P, Q, R, S, T, U, V, W, X, Y, Z
  };
  std::ostream& operator<<(std::ostream& os, const TerminalSymbol& ts);
}

struct math_expression::TerminalSymbol {
  enum Type {
    VARIABLE, VALUE, ARITHMETIC_OPERATOR,
    FUNCTION, OPENING_PARENTHESIS, CLOSING_PARENTHESIS, UN_RECONIZED
  }; 
  TerminalSymbol() : TerminalSymbol(UN_RECONIZED, "", 0) {}
  TerminalSymbol(Type type) : TerminalSymbol(type, "", 0) {}
  TerminalSymbol(Type type, const std::string& value, int column) 
    : type(type), value(value), column(column) {}
  TerminalSymbol(const TerminalSymbol& other) 
    : TerminalSymbol(other.type, other.value, other.column) {}
  bool operator==(const TerminalSymbol& other) const {
    return type == other.type;
  }
  bool operator!=(const TerminalSymbol& other) const {
    return type != other.type;
  }
  const Type type;
  const std::string value;
  const int column = 0;
};

struct math_expression::Symbol {
  Symbol(const TerminalSymbol& value)
    : is_terminal(true), terminal_value(value), non_terminal_value() {}
  Symbol(const NonTerminalSymbol& value) 
    : is_terminal(false), terminal_value(), non_terminal_value(value) {} 
  Symbol(const Symbol& other)
    : is_terminal(other.is_terminal), terminal_value(other.terminal_value),
      non_terminal_value(other.non_terminal_value) {}
  const bool is_terminal;
  const TerminalSymbol terminal_value;
  const NonTerminalSymbol non_terminal_value;
};

#endif // MATH_EXPRESSION_SYMBOL_H
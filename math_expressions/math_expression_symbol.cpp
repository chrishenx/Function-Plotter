#include "math_expression_symbol.h"

using namespace std;

ostream& math_expression::operator<<(ostream& os, const TerminalSymbol& ts) {
  switch (ts.type) {
    case TerminalSymbol::VARIABLE:
      os << "Variable: " << ts.value;
      break;
    case TerminalSymbol::VALUE:
      os << "Value: " << ts.value;
      break;
    case TerminalSymbol::ARITHMETIC_OPERATOR:
      os << "Operator: " << ts.value;
      break;
    case TerminalSymbol::FUNCTION:
      os << "Function: " << ts.value;
      break;
    case TerminalSymbol::OPENING_PARENTHESIS:
      os << "Opening parenthesis";
      break;
    case TerminalSymbol::CLOSING_PARENTHESIS:
      os << "Closing parenthesis";
      break;
    case TerminalSymbol::UN_RECONIZED:
      os << "Unreconized: " << ts.value;
      break;
  }
  return os;
}
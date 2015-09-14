#ifndef MATH_EXPRESSION_PARSER_H
#define MATH_EXPRESSION_PARSER_H

#include <string>
#include <vector>
#include <list>

#include "math_expression_symbol.h"
#include "math_expression_functions.h"

namespace math_expression {
  class Parser;
  struct Operand;
  struct Operation;
  typedef TerminalSymbol Token;
  typedef std::vector<Token> Tokens;
  typedef std::list<Operation> Instructions;
}

struct math_expression::Operand {
  Operand() : Operand(false, "0") {}
  Operand(bool is_value, const std::string& value)
    : is_value(is_value), value(value) {}
  const bool is_value; // is not, value is adress
  const std::string value;
};

struct math_expression::Operation {
  Operation(const Operand& left, const Operand& right, 
      int result_adress, const Function& function) 
    : left(left), right(right), result_adress(result_adress), function(function) {}
  Operation(const Operand& op, int result_adress, const Function& function) 
    : left(op), right(), result_adress(result_adress), function(function) {}
  const Operand left;
  const Operand right;
  const int result_adress;
  const Function function;
};

class math_expression::Parser {
 public:
  enum Error {
    LEXICAL, GRAMMAR, NON
  }; 
  explicit Parser(const std::string& expression) 
    : mExpression(expression), mError(Error::NON) {}
  bool validate();
  Error error() const { return mError; }
  Tokens lexical_analysis();
  void sintax_analysis(const Tokens& tokens);
  Instructions generate_algorithm(const Tokens& tokens) const;

  static const std::string FUNCTIONS[18];
 private:
  Tokens tokens_without_parenthesis(const Tokens& tokens, 
    std::vector<int>& levels, int& max_level) const;
  Function make_function(const std::string& func) const;
  bool isFunction(const std::string& str) const;
  const std::string mExpression;
  Error mError;
  bool analized;
};

#endif // MATH_EXPRESSION_PARSER_H

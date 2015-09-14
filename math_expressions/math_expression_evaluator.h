#ifndef MATH_EXPRESSION_EVALUATOR_H
#define MATH_EXPRESSION_EVALUATOR_H

#include "math_expression_parser.h"
#include "math_expression_symbol.h"

#include <unordered_map>

namespace math_expression {
  class Evaluator;
}

class math_expression::Evaluator {
 public:
  Evaluator(const Instructions& instructions, const Tokens& tokens = {});
  ~Evaluator() { delete[] adress; }
  void set_variable_value(char var, double value) { variables[var] = value; }
  double evaluate();
  bool expression_is_constant() const { return is_constant; }
 private:
  double variable_value(char var) {
    if (variables.find(var) == variables.end()) return 0;
    return variables[var];
  }

  Instructions instructions;
  std::unordered_map<char, double> variables;
  bool is_constant; // When the tokes have no variables
  double* adress;
  double last_evaluation;
};

#endif // MATH_EXPRESSION_EVALUATOR_H

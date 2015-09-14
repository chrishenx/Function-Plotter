#include "math_expression_evaluator.h"
#include <string>
#include <cstdlib>

using namespace math_expression;


Evaluator::Evaluator(const Instructions& instructions, const Tokens& tokens) 
    : instructions(instructions), is_constant(true), 
      adress(new double[instructions.size()]) {
  if (!tokens.empty()) {
    for (const Token& token : tokens) {
      if (token.type == TerminalSymbol::VARIABLE) {
        is_constant = false;
        break;
      }
    }
    if (is_constant) {
      is_constant = false;
      evaluate();
      is_constant = true;
    }
  } else {
    is_constant = false;
  }
}

double Evaluator::evaluate() {
  if (!is_constant) {
    for (const Operation& operation : instructions) {
      const Operand& left = operation.left;
      const Operand& right = operation.right;
      double left_value = 0;
      double right_value = 0;
      if (isalpha(left.value[0])) {
        left_value = variable_value(left.value[0]);
      } else {
        left_value = stod(left.value);
      }
      if (isalpha(right.value[0])) {
        right_value = variable_value(right.value[0]);
      } else {
        right_value = stod(right.value);
      }
      if (!left.is_value) left_value = adress[int(left_value)];
      if (!right.is_value) right_value = adress[int(right_value)];
      const int result_adress = operation.result_adress;
      adress[result_adress] = operation.function(left_value, right_value);
    }
    last_evaluation = adress[instructions.size() - 1];
  }
  return last_evaluation;
}

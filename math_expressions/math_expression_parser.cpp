#include "math_expression_parser.h"

#include <vector>
#include <stack>
#include <queue>
#include <cstdlib>

using namespace std;
using namespace math_expression;

bool Parser::validate() {
  Tokens tokens = lexical_analysis();
  if (mError == NON) {
    sintax_analysis(tokens);
    if (mError == NON) {
      return true;
    }
  }
  return false;
}

const std::string Parser::FUNCTIONS[18] = {
  "sin", "cos", "tan", "asin", "acos", "atan", // Trigonometric
  "sinh", "cosh", "tanh", "acosh", "asinh", "atanh", // Hyperbolic
  "exp", "ln", "log", "sqrt", "abs", "cbrt" // Miscelaneous
};

bool Parser::isFunction(const string& str) const {
  for (const string& function : FUNCTIONS) {
    if (function == str) {
      return true;
    }
  }
  return false;
}

enum State {
  BEGIN, VAR, FUNC, OK, NUM1, TRANS, NUM2
};

static TerminalSymbol make_onechar_token(const string& str, int column) {
  char symbol = str[0];
  if (string("+-*/^").find(symbol) != string::npos) {
    return TerminalSymbol(TerminalSymbol::ARITHMETIC_OPERATOR, str, column);
  } else if (symbol == '(') {
    return TerminalSymbol(TerminalSymbol::OPENING_PARENTHESIS, str, column);
  } else if (symbol == ')') {
    return TerminalSymbol(TerminalSymbol::CLOSING_PARENTHESIS, str, column);
  } 
  return {};
}

/*
  Automaton as tokens generator and patterns validator

    Z = {[Aa-Zz], [0-9], +,-,*,^,(,)}
    states = {BEGIN, VAR, FUNC, NUM1, TRANS, NUM2, OK}
    aceptors = {VAR, FUNC, NUM1, NUM2, OK}

           [Aa-Zz] |  [0-9] |    .    |  +,-,*,^,(,)      
    BEGIN |  VAR   |  NUM1  |         |      OK
    VAR   |  FUNC  |        |         |
    FUNC  |  FUNC  |        |         |
    NUM1  |        |  NUM1  |  TRANS  |
    TRANS |        |  NUM2  |         |
    NUM2  |        |  NUM2  |         |
    OK    |        |        |         |  
  
  All the aceptors return to BEGIN when there are not a transition with
  the symbol currently read, and that symbol is not consumed. So, the 
  complexity of the analisys is at most F(2n) 

  @author Christian González León
 */
Tokens Parser::lexical_analysis() {
  if (mExpression.empty()) {
    mError = LEXICAL;
    return {};
  }

  Tokens tokens;
  State current_state = BEGIN;
  const int N = mExpression.length();
  string aux;
  aux.reserve(N);
  bool ok = true;
  int i = 0;
  while (i <= N && ok) {
    char symbol = i < N ? mExpression[i] : ' ';
    switch (current_state) {
    case BEGIN:
      if (isalpha(symbol)) {
        current_state = VAR;
        aux += symbol;
      } else if (isdigit(symbol)) {
        current_state = NUM1;
        aux += symbol;
      } else if (string("+-*/^()").find(symbol) != string::npos) {
        current_state = OK;
        aux += symbol;
      } else if (symbol != ' ') {
        if (i < N) {
          ok = false;
        }
      }
      i++;
      break;
    case VAR:
      if (isalpha(symbol)) {
        current_state = FUNC;
        aux += symbol;
        i++;
      } else {
        tokens.push_back(TerminalSymbol(TerminalSymbol::VARIABLE, aux, i));
        aux.clear();
        current_state = BEGIN;
      }
      break;
    case FUNC:
      if (isalpha(symbol)) {
        aux += symbol;
        i++;
      } else {
        if (isFunction(aux)) {
          tokens.push_back(TerminalSymbol(TerminalSymbol::FUNCTION, aux, i));
          aux.clear();
          current_state = BEGIN;
        } else {
          ok = false;
        }
      }
      break;
    case OK:
      tokens.push_back(make_onechar_token(aux, i));
      aux.clear();
      current_state = BEGIN;
      break;
    case NUM1:
      if (isdigit(symbol)) {
        aux += symbol;
        i++;
      } else if (symbol == '.') {
        aux += symbol;
        i++;
        current_state = TRANS;   
      } else {
        tokens.push_back(TerminalSymbol(TerminalSymbol::VALUE, aux, i));
        aux.clear();
        current_state = BEGIN;
      }
      break;
    case TRANS:
      if (isdigit(symbol)) {
        current_state = NUM2;
        aux += symbol;
        i++;
      } else {
        ok = false;
      }
      break;
    case NUM2:
      if (isdigit(symbol)) {
        aux += symbol;
        i++;
      } else {
        tokens.push_back(TerminalSymbol(TerminalSymbol::VALUE, aux, i));
        aux.clear();
        current_state = BEGIN;
      }
      break;
    }
  }
  if (!ok) {
    mError = Error::LEXICAL;
  } 
  return tokens;
}


/*
  LL1 Grammar for mathematical expressions
  
  E -> TX
  T -> F(E) | (E)X | NX | VX | +U | -U
  X -> +U | -U | *U | /U | ^U | e
  U -> F(E) | (E)X | NX | VX

  e = Epsilon
  F = Function (sin, cos, ...)
  O = Arithmetic operator (^|+|-|/|*)
  N = Number
  V = Variable (one letter)

  This analisys is based in a depth-first search in a graph
  where the nodes that could spand are NonTerminalSymbols and
  their neighbours are their productions, so eather NonTerminalSymbols 
  or TerminalSymbols.

  @author Christian González León
 */
void Parser::sintax_analysis(const Tokens& tokens) {
  stack<Symbol> stk;
  stk.push(NonTerminalSymbol::E);
  const int N = tokens.size();
  int position = 0;
  while (!stk.empty()) {
    const Symbol& current_symbol = stk.top();
    stk.pop();
    if (!current_symbol.is_terminal) {
      NonTerminalSymbol value = current_symbol.non_terminal_value;
      if (value == E) {
        stk.push(NonTerminalSymbol::X);
        stk.push(NonTerminalSymbol::T);
      } else if (value == T) {
        if (tokens[position] == TerminalSymbol::FUNCTION) {
          stk.push(TerminalSymbol(TerminalSymbol::CLOSING_PARENTHESIS));
          stk.push(NonTerminalSymbol::E);
          stk.push(TerminalSymbol(TerminalSymbol::OPENING_PARENTHESIS));
          stk.push(TerminalSymbol(TerminalSymbol::FUNCTION));
        } else if (tokens[position] == TerminalSymbol::OPENING_PARENTHESIS) {
          stk.push(NonTerminalSymbol::X);
          stk.push(TerminalSymbol(TerminalSymbol::CLOSING_PARENTHESIS));
          stk.push(NonTerminalSymbol::E);
          stk.push(TerminalSymbol(TerminalSymbol::OPENING_PARENTHESIS));
        } else if (tokens[position] == TerminalSymbol::VALUE) {
          stk.push(NonTerminalSymbol::X);
          stk.push(TerminalSymbol(TerminalSymbol::VALUE));
        } else if (tokens[position] == TerminalSymbol::VARIABLE) {
          stk.push(NonTerminalSymbol::X);
          stk.push(TerminalSymbol(TerminalSymbol::VARIABLE)) ;
        } else if (tokens[position].value[0] == '-' || 
            tokens[position].value[0] == '+') {
          stk.push(NonTerminalSymbol::U);
          stk.push(TerminalSymbol(TerminalSymbol::ARITHMETIC_OPERATOR));
        } else {
          mError = GRAMMAR;
          break;
        }
      } else if (value == X) {
        if (position >= N) {
        } else if (tokens[position] == TerminalSymbol::ARITHMETIC_OPERATOR) {
          stk.push(NonTerminalSymbol::U);
          stk.push(TerminalSymbol(TerminalSymbol::ARITHMETIC_OPERATOR));
        } else if (tokens[position] != TerminalSymbol::CLOSING_PARENTHESIS) {
          mError = GRAMMAR;
          break;
        } else {
        }
      } else if (value == U) {
        if (tokens[position] == TerminalSymbol::FUNCTION) {
          stk.push(TerminalSymbol(TerminalSymbol::CLOSING_PARENTHESIS));
          stk.push(NonTerminalSymbol::E);
          stk.push(TerminalSymbol(TerminalSymbol::OPENING_PARENTHESIS));
          stk.push(TerminalSymbol(TerminalSymbol::FUNCTION));
        } else if (tokens[position] == TerminalSymbol::OPENING_PARENTHESIS) {
          stk.push(NonTerminalSymbol::X);
          stk.push(TerminalSymbol(TerminalSymbol::CLOSING_PARENTHESIS));
          stk.push(NonTerminalSymbol::E);
          stk.push(TerminalSymbol(TerminalSymbol::OPENING_PARENTHESIS));
        } else if (tokens[position] == TerminalSymbol::VALUE) {
          stk.push(NonTerminalSymbol::X);
          stk.push(TerminalSymbol(TerminalSymbol::VALUE));
        } else if (tokens[position] == TerminalSymbol::VARIABLE) {
          stk.push(NonTerminalSymbol::X);
          stk.push(TerminalSymbol(TerminalSymbol::VARIABLE)) ;
        } else {
          mError = GRAMMAR;
          break;
        } 
      }
    } else {
      const TerminalSymbol& value = current_symbol.terminal_value;
      if (value == tokens[position]) {
        position++;
      } else {
        mError = GRAMMAR;
        break;
      }
    }
  }
  if (position != N) mError = GRAMMAR;
}

/*
  - This method generates a set of linear instructions which 
    will be used for an evaluator in order to calculate the result
    of the math-expression.
  - The instructions are optimised since there are no need of temporary
    values, just temporaty results. 
    This is possible because the operations are produced to be 
    stored in adresses generated at time.
    This code can potentially raise to generete essambler code.
    Each instrucciones stores the left and right operands and a functor
    which then will operate them.

  @author Christian González León
 */
Instructions Parser::generate_algorithm(const Tokens& tokens) const {
  vector<int> levels; // Depth of each new_token 
  int max_level = 0; // (E) + (E) = 1 | ((E)) = 2 | (E) + ((E)) = 2
  Tokens new_tokens = tokens_without_parenthesis(tokens, levels, max_level);
  const int N = new_tokens.size();
  vector<bool> evaluated(N, false);
  vector<int> adress(N, -1); // Adress of temporaty results
  int last_adress = 0; 
  Instructions instructions;
  list<string> instructions_name;
  int left = 0; // left hand operator index
  int right = 0; // right hand operator index
  int left_is_value = false; // if true, it is adress
  int right_is_value = false; // if true, it is adress

  if (N == 1) {
    Operand operand(true, new_tokens[0].value);
    instructions.push_back(Operation(operand, 0, unary_addition));
    instructions_name.push_back("+ Unary");
  } else // I dont want any extra identation 

  for (int current_level = max_level; current_level >= 0; current_level--) {
    for (int i = 0; i < N; i++) {
      if (levels[i] == current_level && new_tokens[i] == Token::FUNCTION) {
        for (right = i + 1; right < N; right++)
          if(!evaluated[right]) break;
        evaluated[right] = true;
        const Token& right_token = new_tokens[right];
        right_is_value = adress[right] == -1;
        Operand operand(right_is_value,
          right_is_value ? right_token.value : to_string(adress[right]));
        instructions.push_back(Operation(operand, last_adress, 
          make_function(new_tokens[i].value)));
        instructions_name.push_back(new_tokens[i].value);
        adress[i] = last_adress; 
        last_adress++;
      }
    }
    
    for (int i = 0; i < N; i++) {
      if (levels[i] == current_level && new_tokens[i].value[0] == '^') {
        for (left = i - 1; left > 0; left--) 
          if(!evaluated[left]) break;
        for (right = i + 1; right < N; right++)
          if(!evaluated[right]) break;
        evaluated[i] = true;
        evaluated[right] = true;
        const Token& left_token = new_tokens[left];
        const Token& right_token = new_tokens[right];
        left_is_value = adress[left] == -1;
        right_is_value = adress[right] == -1;
        Operand left_operand(left_is_value, 
          left_is_value ? left_token.value : to_string(adress[left]));
        Operand right_operand(right_is_value, 
          right_is_value ? right_token.value : to_string(adress[right]));
        adress[left] = last_adress;
        instructions.push_back(Operation(left_operand, right_operand, last_adress, pow));
        instructions_name.push_back(new_tokens[i].value);
        last_adress++;
      }
    }

    for (int i = 0; i < N; i++) {
      if (levels[i] == current_level && (new_tokens[i].value[0] == '*' ||
          new_tokens[i].value[0] == '/')) {
        for (left = i - 1; left > 0; left--) 
          if(!evaluated[left]) break;
        for (right = i + 1; right < N; right++)
          if(!evaluated[right]) break;
        evaluated[i] = true;
        evaluated[right] = true;
        const Token& left_token = new_tokens[left];
        const Token& right_token = new_tokens[right];
        left_is_value = adress[left] == -1;
        right_is_value = adress[right] == -1;
        Operand left_operand(left_is_value, 
          left_is_value ? left_token.value : to_string(adress[left]));
        Operand right_operand(right_is_value, 
          right_is_value ? right_token.value : to_string(adress[right]));
        adress[left] = last_adress;
        instructions.push_back(
          Operation(left_operand, right_operand, last_adress, 
            new_tokens[i].value[0] == '*' ? multiplication : division
          ));
        instructions_name.push_back(new_tokens[i].value);
        last_adress++;
      }
    }

    for (int i = 0; i < N; i++) {
      if (levels[i] == current_level && (new_tokens[i].value[0] == '+' ||
          new_tokens[i].value[0] == '-')) {
        for (left = i - 1; left > 0; left--) 
          if(!evaluated[left]) break;
        for (right = i + 1; right < N; right++)
          if(!evaluated[right]) break;
        evaluated[right] = true;
        const Token& left_token = new_tokens[left];
        const Token& right_token = new_tokens[right];
        left_is_value = adress[left] == -1;
        right_is_value = adress[right] == -1;
        Operand left_operand(left_is_value, 
          left_is_value ? left_token.value : to_string(adress[left]));
        Operand right_operand(right_is_value, 
          right_is_value ? right_token.value : to_string(adress[right]));
        if (i == 0 || levels[i - 1] < current_level) {
          // Unary +|-
          adress[i] = last_adress;
          instructions.push_back(Operation(right_operand, last_adress, 
            new_tokens[i].value[0] == '+' ? unary_addition : unary_subtraction));
          instructions_name.push_back(new_tokens[i].value + " unary");
        } else {
          evaluated[i] = true;
          adress[left] = last_adress;
          // Binary +|-
          instructions.push_back(Operation(left_operand, right_operand, last_adress, 
            new_tokens[i].value[0] == '+' ? binary_addition : binary_subtraction));
          instructions_name.push_back(new_tokens[i].value + " binary");
        }
        last_adress++; 
      }
    }
  }
  return instructions;
}

Tokens Parser::tokens_without_parenthesis(const Tokens& tokens, 
    vector<int>& levels, int& max_level) const {
  const int N = tokens.size();
  Tokens new_tokens;
  new_tokens.reserve(N);
  levels.reserve(N);
  max_level = 0;
  for (int i = 0, level = 0; i < N; i++) {
    if (tokens[i] == TerminalSymbol::OPENING_PARENTHESIS) {
      level++;
      if (level > max_level) max_level = level;
    } else if (tokens[i] == TerminalSymbol::CLOSING_PARENTHESIS) {
      level--;
    } else {
      levels.push_back(level);
      new_tokens.push_back(tokens[i]);
    }
  }
  return new_tokens;
}

Function Parser::make_function(const std::string& func) const {
  Function function = nullptr;
       if (func == "sin")   function = sin;
  else if (func == "cos")   function = cos;
  else if (func == "tan")   function = tan;
  else if (func == "asin")  function = asin;
  else if (func == "acos")  function = acos;
  else if (func == "atan")  function = atan;
  else if (func == "sinh")  function = sinh;
  else if (func == "cosh")  function = cosh;
  else if (func == "tanh")  function = tanh;
  else if (func == "acosh") function = acosh;
  else if (func == "asinh") function = asinh;
  else if (func == "atanh") function = atanh;
  else if (func == "exp")   function = exp;
  else if (func == "ln")    function = log;
  else if (func == "log")   function = log10;
  else if (func == "sqrt")  function = sqrt;
  else if (func == "abs")   function = abs;
  else if (func == "cbrt")  function = cbrt;
  return function;
}

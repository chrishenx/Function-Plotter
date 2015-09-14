#ifndef MATH_EXPRESSION_FUNCTIONS_H
#define MATH_EXPRESSION_FUNCTIONS_H

#include <cmath>

namespace math_expression {
  typedef double(*Function)(double, double); 

  inline double binary_addition(double a, double b) {
    return a + b;
  }
  inline double binary_subtraction(double a, double b) {
    return a - b;
  }
  inline double unary_addition(double a, double /*unused*/) {
    return a;
  }
  inline double unary_subtraction(double a, double /*unused*/) {
    return -a;
  }
  inline double multiplication(double a, double b) {
    return a * b;
  }
  inline double division(double a, double b) {
    return a / b;
  }
  // The pow function is already in cmath
  // sin, cos, ..., etc spect one argument but I need two
  inline double sin(double x, double /*unused*/) {
    return ::sin(x);
  }
  inline double cos(double x, double /*unused*/) {
    return ::cos(x);
  }
  inline double tan(double x, double /*unused*/) {
    return ::tan(x);
  }
  inline double asin(double x, double /*unused*/) {
    return ::asin(x);
  }
  inline double acos(double x, double /*unused*/) {
    return ::acos(x);
  }
  inline double atan(double x, double /*unused*/) {
    return ::atan(x);
  }
  inline double sinh(double x, double /*unused*/) {
    return ::sinh(x);
  }
  inline double cosh(double x, double /*unused*/) {
    return ::cosh(x);
  }
  inline double tanh(double x, double /*unused*/) {
    return ::tanh(x);
  }
  inline double asinh(double x, double /*unused*/) {
    return ::asinh(x);
  }
  inline double acosh(double x, double /*unused*/) {
    return ::acosh(x);
  }
  inline double atanh(double x, double /*unused*/) {
    return ::tanh(x);
  }
  inline double exp(double x, double /*unused*/) {
    return ::exp(x);
  }
  inline double log(double x, double /*unused*/) {
    return ::log(x);
  }
  inline double log10(double x, double /*unused*/) {
    return ::log10(x);
  }
  inline double sqrt(double x, double /*unused*/) {
    return ::sqrt(x);
  }
  inline double abs(double x, double /*unused*/) {
    return ::abs(x);
  }
  inline double cbrt(double x, double /*unused*/) {
    return ::cbrt(x);
  }
} 

#endif // MATH_EXPRESSION_FUNCTIONS_H
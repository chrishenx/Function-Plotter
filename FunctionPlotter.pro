#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T13:37:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = FunctionPlotter
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    math_expressions/math_expression_evaluator.cpp \
    math_expressions/math_expression_parser.cpp \
    math_expressions/math_expression_symbol.cpp

HEADERS  += mainwindow.h \
    qcustomplot/qcustomplot.h \
    math_expressions/math_expression_evaluator.h \
    math_expressions/math_expression_functions.h \
    math_expressions/math_expression_parser.h \
    math_expressions/math_expression_symbol.h

FORMS    += mainwindow.ui

RC_FILE = ch_p_icon.rc

RESOURCES += \
    resources.qrc

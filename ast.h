#ifndef AST_H
#define AST_H
/* Abstract Syntax Tree */

#include <string>
#include <ostream>
#include <iostream>
#include "vartype.h"
#include "value.h"
#include "parseessentials.h"

class AST{

};

class Statement : public AST{

};

class TypeDefAST : public Statement{
public:
};

class Expression : public Statement{

};

class Value : public Expression{

};

class Identifier : public Value{

};

class Let : public Statement{
public:
    Let(Identifier identifier, Expression expression): identifier(identifier), expression(expression){}

    Identifier identifier;
    Expression expression;
};

class LetIn : public Expression{
    Let(Identifier identifier, Expression expression, Expression in_expression): identifier(identifier), expression(expression), in_expression(in_expression){}

    Identifier identifier;
    Expression expression;
    Expression in_expression;
};

class FunctionCall : public Expression{

};

class MatchWith : public Expression{

};

class Function : public Value{

};

class ComplexValue : public Value{

};

class Primitive : public Value{

};

class Integer : public Primitive{

};

class Float : public Primitive{

};

class Bool : public Primitive{

};

class String : public Primitive{

};

#endif // AST_H

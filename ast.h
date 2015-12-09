#ifndef AST_H
#define AST_H
/* Abstract Syntax Tree */

#include <string>
#include <ostream>
#include <iostream>
#include <functional>
#include "environment.h"
#include "vartype.h"
#include "value.h"
#include "parseessentials.h"

class AST{
public:

    virtual std::string print(int indents) = 0;
};

class Statement : public AST{

};

class TypeDefAST : public Statement{
public:
};

class Expression : public Statement{

};

class Value : public Expression{
public:

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Somevalue\n");}
};

class Identifier : public Value{
public:
    Identifier(std::string name): name(name) {}

    std::string name;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Identifier: ") + name + "\n";}
};

class Let : public Statement{
public:
    Let(Identifier* identifier, Expression* expression): identifier(identifier), expression(expression){}

    Identifier* identifier;
    Expression* expression;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Let: \n") + identifier->print(indents+1) + expression->print(indents+1);}
};

class LetIn : public Expression{
    LetIn(Identifier* identifier, Expression* expression, Expression* in_expression): identifier(identifier), expression(expression), in_expression(in_expression){}

    Identifier* identifier;
    Expression* expression;
    Expression* in_expression;
};

class Conditional : public Expression{
public:
    Conditional(Expression* condition, Expression* true_path, Expression* false_path): condition(condition), true_path(true_path), false_path(false_path){}

    Expression* condition;
    Expression* true_path;
    Expression* false_path;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Conditional: \n") + condition->print(indents+1) + true_path->print(indents+1) + false_path->print(indents+1);}
};

class FunctionCall : public Expression{
public:
    FunctionCall(Expression* function_expression, Expression* argument_expression): function_expression(function_expression), argument_expression(argument_expression) {}

    Expression* function_expression;
    Expression* argument_expression;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Function Call: \n") + function_expression->print(indents+1) + argument_expression->print(indents+1);}
};

class MatchWith : public Expression{

};

class Function : public Value{
public:
    Function(std::function<Value(Value)> fun): fun(fun) {}
    Function(Identifier* arg_name, Expression* function_expression){
        fun = [arg_name, function_expression](Value argument)->Value {
            return argument;// as for now
        }; // lambda
    } // constructor

    std::function<Value(Value)> fun;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Function \n");}
};

class ComplexValue : public Value{

};

class Primitive : public Value{

};

class Integer : public Primitive{
public:
    Integer(int value): value(value){}

    int value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Integer: ") + std::to_string(value) + "\n";}
};

class Float : public Primitive{
public:
    Float(float value): value(value){}

    float value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Float: ") + std::to_string(value) + "\n";}
};

class Bool : public Primitive{
public:
    Bool(bool value): value(value){}

    bool value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Bool: ") + std::to_string(value) + "\n";}
};

class String : public Primitive{
public:
    String(std::string value): value(value){}

    std::string value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("String: ") + value + "\n";}
};

#endif // AST_H

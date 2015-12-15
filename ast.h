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

class Environment;
class Value;

class AST{
public:

    virtual std::string print(int indents) = 0;
    virtual Type deduceType(Environment& env) = 0;
    virtual Type execute(Environment& env) = 0;
};

class Statement : public AST{
public:
};

class TypeDefAST : public Statement{
public:
};

class Expression : public Statement{
public:
    Type exp_type; // UNDETERMINED by default

    virtual Type deduceType(Environment &env){ return exp_type; }
    virtual Value* call(Environment& env, Expression* argument){ throw std::runtime_error("expression not callable"); }
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

    virtual Type deduceType(Environment& env){
        exp_type = env.getIdentifierType(*this);
        return exp_type;
    }
};

class Let : public Statement{
public:
    Let(Identifier* identifier, Expression* expression): identifier(identifier), expression(expression){}

    Identifier* identifier;
    Expression* expression;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Let: \n") + identifier->print(indents+1) + expression->print(indents+1);}
    virtual Type deduceType(Environment& env){
        env.addIdentifierToBeTypeDeduced(*identifier);
        env.setIdentifierType(*identifier, expression->deduceType(env));
        //return Type();
        return env.getIdentifierType(*identifier);
    }

    virtual Type execute(Environment& env){
        env.addValue(*identifier, expression->execute(env));
    }
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

    virtual Type deduceType(Environment &env){
        env.addActivationFrame();
            function_expression->deduceType(env);
            if(function_expression->exp_type.type_enum != FUNCTION_TYPE) throw std::runtime_error("expression is not a function");
            this->exp_type = function_expression->exp_type.withTypeSwapped(function_expression->exp_type.aggregated_types[0], argument_expression->deduceType(env)).aggregated_types[1]; // take return type with argument type applied
        env.removeActivationFrame();
        return exp_type;
    }
};

class MatchWith : public Expression{

};

class Function : public Value{
public:
    Function(Identifier* arg_name, Expression* function_expression):arg_name(arg_name), function_expression(function_expression)
    {} // constructor

    Identifier* arg_name;
    Expression* function_expression;

    virtual Value* call(Environment& env, Expression* argument){
        env.addActivationFrame();
        //env add argument under identifier
        // execute function_expression
        // return what it returns
        env.removeActivationFrame();
    }

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Function \n");}

    virtual Type deduceType(Environment &env){
        env.addActivationFrame();
            env.addIdentifierToBeTypeDeduced(*arg_name, false);
            function_expression->deduceType(env);
            arg_name->exp_type = env.getIdentifierType(*arg_name);
            this->exp_type = Type(FUNCTION_TYPE, "","",std::vector<Type>{arg_name->exp_type, function_expression->exp_type} );
        env.removeActivationFrame();
        return exp_type;
    }
};

class BuiltIn_Function : public Value{
public:
    BuiltIn_Function(std::function<Value*(Value*)> fun, Type argument_type, Type return_type): fun(fun) { exp_type = Type(FUNCTION_TYPE,"","",std::vector<Type>{argument_type, return_type});}
    std::function<Value*(Value*)> fun;

    virtual Value* call(Environment& , Expression* argument){
        //return fun(argument);
    }
};

class ComplexValue : public Value{

};

class Primitive : public Value{
public:
};

class Integer : public Primitive{
public:
    Integer(int value): value(value){exp_type = Type(PRIMITIVE, "int");}

    int value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Integer: ") + std::to_string(value) + "\n";}
};

class Float : public Primitive{
public:
    Float(float value): value(value){exp_type = Type(PRIMITIVE, "float");}

    float value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Float: ") + std::to_string(value) + "\n";}
};

class Bool : public Primitive{
public:
    Bool(bool value): value(value){exp_type = Type(PRIMITIVE, "bool");}

    bool value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("Bool: ") + std::to_string(value) + "\n";}
};

class String : public Primitive{
public:
    String(std::string value): value(value){exp_type = Type(PRIMITIVE, "string");}

    std::string value;

    virtual std::string print(int indents){return std::string(indents, ' ') + std::string("String: ") + value + "\n";}
};

#endif // AST_H

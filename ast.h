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
    virtual Type deduceType(Environment& env, Type mostGeneralExpected) = 0;
    virtual Value* execute(Environment& env) = 0;
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

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override { return exp_type.getMoreSpecific(mostGeneralExpected); }
    virtual Value* call(Environment& env, Expression* argument){ throw std::runtime_error("expression not callable"); }
};

class Value : public Expression{
public:

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Somevalue\n");}

    virtual Value* execute(Environment& ) override {
        return this;
    }
};

class Identifier : public Value{
public:
    Identifier(std::string name): name(name) {}

    std::string name;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Identifier: ") + name + "\n";}

    virtual Type deduceType(Environment& env, Type mostGeneralExpected) override {
        exp_type = env.getIdentifierType(*this);
        Type newType = exp_type.getMoreSpecific(mostGeneralExpected);
        if(exp_type != newType) env.setIdentifierType(*this, newType);
        return exp_type = newType;
    }

    virtual Value* execute(Environment& env) override {
        return env.getValue(*this);
    }
};

class Let : public Statement{
public:
    Let(Identifier* identifier, Expression* expression): identifier(identifier), expression(expression){}

    Identifier* identifier;
    Expression* expression;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Let: \n") + identifier->print(indents+1) + expression->print(indents+1);}
    virtual Type deduceType(Environment& env, Type) override {
        env.addIdentifierToBeTypeDeduced(*identifier);
        env.setIdentifierType(*identifier, expression->deduceType(env, Type()));
        //return Type();
        return env.getIdentifierType(*identifier);
    }

    virtual Value* execute(Environment& env) override {
        env.addValue(*identifier, expression->execute(env));
        return nullptr;
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

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Conditional: \n") + condition->print(indents+1) + true_path->print(indents+1) + false_path->print(indents+1);}

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        condition->deduceType(env, Type(PRIMITIVE, "bool"));
        Type trueType = mostGeneralExpected;
        Type falseType = mostGeneralExpected;
        do{
            trueType = true_path->deduceType(env, falseType);
            falseType = false_path->deduceType(env, trueType);
        } while(trueType != falseType);
        return this->exp_type = trueType;
    }

    virtual Value* execute(Environment& env) override;
};

class FunctionCall : public Expression{
public:
    FunctionCall(Expression* function_expression, Expression* argument_expression): function_expression(function_expression), argument_expression(argument_expression) {}

    Expression* function_expression;
    Expression* argument_expression;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Function Call: \n") + function_expression->print(indents+1) + argument_expression->print(indents+1);}

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        function_expression->deduceType(env, Type(FUNCTION_TYPE,"","",std::vector<Type>{Type(), Type()}));
        if(function_expression->exp_type.type_enum != FUNCTION_TYPE){
            throw std::runtime_error("expression is not a function");
        }
        this->exp_type = function_expression->exp_type.withTypeSwapped(function_expression->exp_type.aggregated_types[0],
                function_expression->exp_type.aggregated_types[0].getMoreSpecific(argument_expression->deduceType(env, function_expression->exp_type.aggregated_types[0])) ).aggregated_types[1]; // take return type with argument type applied
        return exp_type;
    }

    virtual Value* execute(Environment& env) override {
        Value* function_val = function_expression->execute(env);
        return function_val->call(env, argument_expression);;
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
    Environment env_copy;

    virtual Value* call(Environment& env, Expression* argument) override {
        // function internal expression works on env_copy!
        env_copy.addActivationFrame();
        env_copy.addValue(*arg_name, argument->execute(env));
        Value* return_value = function_expression->execute(env_copy);
        env_copy.removeActivationFrame();
        return return_value;
    }

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Function \n");}

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        env.addActivationFrame();
            env.addIdentifierToBeTypeDeduced(*arg_name, false);
            function_expression->deduceType(env, mostGeneralExpected.getMoreSpecific(Type(FUNCTION_TYPE,"","",std::vector<Type>{Type(),Type()})).aggregated_types[1] );
            arg_name->exp_type = env.getIdentifierType(*arg_name);
            this->exp_type = Type(FUNCTION_TYPE, "","",std::vector<Type>{arg_name->exp_type, function_expression->exp_type} );
        env.removeActivationFrame();
        return exp_type;
    }

    virtual Value* execute(Environment& env) override {
        this->env_copy = env;
        return this;
    }
};


class BuiltIn_Function : public Value{
public:
    BuiltIn_Function(std::function<Value*(Value*)> fun, Type argument_type, Type return_type): fun(fun) { exp_type = Type(FUNCTION_TYPE,"","",std::vector<Type>{argument_type, return_type});}
    std::function<Value*(Value*)> fun;

    virtual Value* call(Environment& env, Expression* argument) override {
        return fun.operator()(argument->execute(env));
    }

    virtual Value* execute(Environment& env) override {
        return this;
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

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Integer: ") + std::to_string(value) + "\n";}
};

class Float : public Primitive{
public:
    Float(float value): value(value){exp_type = Type(PRIMITIVE, "float");}

    float value;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Float: ") + std::to_string(value) + "\n";}
};

class Bool : public Primitive{
public:
    Bool(bool value): value(value){exp_type = Type(PRIMITIVE, "bool");}

    bool value;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Bool: ") + std::to_string(value) + "\n";}
};

class String : public Primitive{
public:
    String(std::string value): value(value){exp_type = Type(PRIMITIVE, "string");}

    std::string value;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("String: ") + value + "\n";}
};

#endif // AST_H

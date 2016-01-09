#ifndef AST_H
#define AST_H
/* Abstract Syntax Tree */

#include <string>
#include <ostream>
#include <iostream>
#include <functional>
#include <cassert>
#include <sstream>
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

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        if(exp_type.type_enum == UNDETERMINED){ exp_type = env.getNewPolymorphicType(); }
        env.addRelation(exp_type, mostGeneralExpected);
        return exp_type = env.followRelations(exp_type);
    }
    virtual Value* call(Environment& env, Value* argument){ throw std::runtime_error("expression not callable"); }

    virtual bool isValue() { return false; }
};

class Value : public Expression{
public:

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Somevalue\n");}

    virtual Value* execute(Environment& ) override {
        return this;
    }

    virtual bool isValue() override { return true; }
};

class Identifier : public Value{
public:
    Identifier(std::string name): name(name) {}

    std::string name;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Identifier: ") + name + "\n";}

    virtual Type deduceType(Environment& env, Type mostGeneralExpected) override {
        Expression::deduceType(env, mostGeneralExpected);
        exp_type = env.followRelations(exp_type);
        env.addRelation(exp_type, env.getIdentifierType(*this));
        env.setIdentifierType(*this, mostGeneralExpected);
        return exp_type;
    }

    virtual Value* execute(Environment& env) override {
        return env.getValue(*this);
    }

    friend bool operator<(const Identifier& lhs, const Identifier& rhs){
        return lhs.name < rhs.name;
    }
};

class Let : public Statement{
public:
    Let(Identifier* identifier, Expression* expression, bool recursive): identifier(identifier), expression(expression), recursive(recursive){}

    Identifier* identifier;
    Expression* expression;
    bool recursive;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + (recursive?std::string("Let rec: \n"):std::string("Let: \n")) + identifier->print(indents+1) + expression->print(indents+1);}
    virtual Type deduceType(Environment& env, Type mostGeneralExpected) override {
        if(identifier->exp_type.type_enum == UNDETERMINED) identifier->exp_type = env.getNewPolymorphicType();
        if(expression->exp_type.type_enum == UNDETERMINED) expression->exp_type = env.getNewPolymorphicType();

        env.addRelation(identifier->exp_type, expression->exp_type);

        if(recursive){
            if(expression->isValue()){
                env.addIdentifierToBeTypeDeduced(*identifier, false, identifier->exp_type);
                expression->deduceType(env, mostGeneralExpected);
                env.setIdentifierType(*identifier, env.followRelations(identifier->exp_type));
            }
            else throw std::runtime_error("using rec keyword on non value");
        }
        else{
            expression->deduceType(env, mostGeneralExpected); // deduce before adding it to identifier list, so it wont be visible recursively
            env.addIdentifierToBeTypeDeduced(*identifier, false, env.followRelations(identifier->exp_type));
        }

        return env.getIdentifierType(*identifier);
    }

    virtual Value* execute(Environment& env) override {
        if(recursive){
            if(expression->isValue()){
                env.addValue(*identifier, static_cast<Value*>(expression));
                expression->execute(env);
                return nullptr;
            }
            else throw std::runtime_error("using rec keyword on non value");
        }
        else{
            Value* expression_result = expression->execute(env);
            env.addValue(*identifier, expression_result);
            return nullptr;
        }
    } // execute
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
        if(exp_type.type_enum == UNDETERMINED) exp_type = mostGeneralExpected;

        env.addRelation(exp_type, mostGeneralExpected);
        true_path->deduceType(env, exp_type);
        false_path->deduceType(env, exp_type);

        return env.followRelations(exp_type);
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
        // only request new polymorphic type if needed
        ///if(function_expression->exp_type.type_enum == FUNCTION_TYPE) function_expression->deduceType(env, Type(FUNCTION_TYPE,"","",std::vector<Type>{Type(), mostGeneralExpected}));
        ///else function_expression->deduceType(env, Type(FUNCTION_TYPE,"","",std::vector<Type>{env.getNewPolymorphicType(), env.getNewPolymorphicType().getMoreSpecific(mostGeneralExpected)}));
        Expression::deduceType(env, mostGeneralExpected);

        Type function_expression_type = function_expression->deduceType(env, Type(FUNCTION_TYPE,"","",std::vector<Type>{env.getNewPolymorphicType(), mostGeneralExpected}));
        Type argument_expression_type = argument_expression->deduceType(env, function_expression->exp_type.aggregated_types[0]);

        env.addFunctionCallRelations(function_expression_type, argument_expression_type, exp_type);

        return exp_type = env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override {
        Value* function_val = function_expression->execute(env);
        return function_val->call(env, argument_expression->execute(env));;
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
    //bool currently_being_called;

    virtual Value* call(Environment&, Value* argument) override {
        // function internal expression works on env_copy!
        //currently_being_called = true;
        Environment working_copy = env_copy;
        working_copy.addActivationFrame();
        working_copy.addValue(*arg_name, argument);
        Value* return_value = function_expression->execute(working_copy);
        working_copy.removeActivationFrame();
        //currently_being_called = false;
        return return_value;
    }

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Function: \n") + arg_name->print(indents+1) + function_expression->print(indents+1);}

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        env.addActivationFrame();
            if(exp_type.type_enum == UNDETERMINED) exp_type = Type(FUNCTION_TYPE, "", "", std::vector<Type>{env.getNewPolymorphicType(), env.getNewPolymorphicType()});
            if(env.followRelations(exp_type).type_enum == POLYMORPHIC){
                env.addRelation(exp_type, Type(FUNCTION_TYPE, "", "", std::vector<Type>{env.getNewPolymorphicType(), env.getNewPolymorphicType()}));
                exp_type = env.followRelations(exp_type);
            }
            env.addRelation(exp_type, mostGeneralExpected);
            env.addIdentifierToBeTypeDeduced(*arg_name, false, exp_type.aggregated_types[0]);
            function_expression->deduceType(env, exp_type.aggregated_types[1]);
        env.removeActivationFrame();
        return env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override {
        //if(currently_being_called) throw std::runtime_error("env copy corruption");
        this->env_copy = env;
        return this;
    }

};


class BuiltIn_Function : public Value{
public:
    BuiltIn_Function(std::function<Value*(Value*)> fun, Type argument_type, Type return_type): fun(fun) { exp_type = Type(FUNCTION_TYPE,"","",std::vector<Type>{argument_type, return_type});}
    std::function<Value*(Value*)> fun;

    virtual Value* call(Environment& env, Value* argument) override {
        return fun.operator()(argument);
    }

    virtual Value* execute(Environment& env) override {
        return this;
    }
};

class ComplexValue : public Value{
public:
    ComplexValue(Type type, std::vector<Value*> aggregatedValues): type(type), aggregatedValues(aggregatedValues){}

    Type type;
    std::vector<Value*> aggregatedValues;

    virtual std::string print(int indents) override {
        std::stringstream ss;
        ss << std::string(indents, ' ') << type << ":" << std::endl;
        for(unsigned int i=0; i<aggregatedValues.size(); ++i) ss << aggregatedValues[i]->print(indents + 1);
        return ss.str();
    }
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

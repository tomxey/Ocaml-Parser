#ifndef AST_H
#define AST_H
/* Abstract Syntax Tree */

#include <string>
#include <ostream>
#include <iostream>
#include <functional>
#include <cassert>
#include <set>
#include <sstream>
#include "environment.h"
#include "vartype.h"
#include "value.h"
#include "parseessentials.h"

//#define USE_STATICALLY_DEDUCED_TYPES

#define UNBOUND_VALUE_VARIABLE_CLASS_ID 0
#define IDENTIFIER_CLASS_ID 1
#define FUNCTION_CLASS_ID 2
#define BUILT_IN_FUNCTION_CLASS_ID 3
#define COMPLEX_VALUE_CLASS_ID 4
#define PRIMITIVE_CLASS_ID 5

#define LIST_LINE_BREAK 10

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
    TypeDefAST(std::string type_name, std::vector<std::string> polymorphic_parameters_names = std::vector<std::string>(), std::vector< std::pair<std::string, Type> > constructors = std::vector< std::pair<std::string, Type> >())
        :type_name(type_name), polymorphic_parameters_names(polymorphic_parameters_names), constructors(constructors){
    }

    std::string type_name;
    std::vector<std::string> polymorphic_parameters_names;
    std::vector< std::pair<std::string, Type> > constructors;

    virtual std::string print(int indents){
        return std::string(indents, ' ') + "TypeDef: " + type_name + "\n";
    }
    virtual Type deduceType(Environment& env, Type);

    virtual Value* execute(Environment& env){
        return nullptr; // return nothing i guess
    }
};

class TypeDefsAST : public Statement{
public:
    TypeDefsAST(std::vector<TypeDefAST*> typeDefs): typeDefs(typeDefs){}
    std::vector<TypeDefAST*> typeDefs;

    virtual std::string print(int indents){
        std::string result = std::string(indents, ' ') + "TypeDefs:\n";
        for(TypeDefAST* def : typeDefs){
            result += def->print(indents+1);
        }
        return result;
    }
    virtual Type deduceType(Environment& env, Type);

    virtual Value* execute(Environment& env){
        return nullptr; // return nothing i guess
    }
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
    virtual bool isValidPattern(std::set<std::string>& ) { throw std::runtime_error("forbidden syntax element inside of pattern"); }
    virtual bool isIdentifier(){return false;}
    virtual bool isNonBuiltInFunction(){return false; }
};

class Value : public Expression{
public:

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Somevalue\n");}
    virtual std::string printValue(){ return std::string("Somevalue"); }

    /*virtual std::string print_with_type_forced(Type type, int indents = 0){
        if(exp_type != type) throw std::runtime_error("forcing incompatible type. " + exp_type.to_string() + " != " + type.to_string());
        else print(indents);
    }*/

    virtual Value* execute(Environment& ) override {
        return this;
    }

    virtual bool isValue() override { return true; }

    virtual bool isValidPattern(std::set<std::string>&) override{return true;}

    /// IMPORTANT!!! draw a graph of all Value classes, and resolve the problem
    virtual bool equals(Value* other);
    virtual bool smallerThan(Value* other);
    virtual bool matchWithValue(Value* other);
    virtual void applyMatch(Value* other, Environment& env);

    virtual int getValueClassID() = 0; // should it be a method? or a field?
};

class UnboundPatternVariable : public Value{
public:

    UnboundPatternVariable(std::string name): name(name) {}

    std::string name;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("UnboundPatternVariable: ") + name + "\n";}
    virtual std::string printValue() override { return name; }

    virtual Type deduceType(Environment&, Type) override {
        throw std::runtime_error("deduceType() function shoudn't be called for UnboundPatternVariable");
    }

    virtual Value* execute(Environment& env) override {
        throw std::runtime_error("execute() function shoudn't be called for UnboundPatternVariable");
    }

    friend bool operator<(const UnboundPatternVariable& lhs, const UnboundPatternVariable& rhs){
        return lhs.name < rhs.name;
    }

    virtual int getValueClassID() override{return UNBOUND_VALUE_VARIABLE_CLASS_ID;}
};

class Identifier : public Value{
public:
    Identifier(std::string name): name(name) {}

    std::string name;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Identifier: ") + name + "\n";}
    virtual std::string printValue() override { return name; }

    virtual Type deduceType(Environment& env, Type mostGeneralExpected) override {
        if(name == "_"){
            if(env.execution_inside_pattern) return exp_type = mostGeneralExpected;
            else throw std::runtime_error("using wildcard _ outside of pattern");
        }
        else{
            Expression::deduceType(env, mostGeneralExpected);
            exp_type = env.followRelations(exp_type);
            env.addRelation(exp_type, env.getIdentifierType(*this));
            env.setIdentifierType(*this, mostGeneralExpected);
            return exp_type = env.followRelations(exp_type);
        }
    }

    virtual Value* execute(Environment& env) override {
        if(env.execution_inside_pattern && std::isupper(name[0]) == false){
            return new UnboundPatternVariable(name);// only identifiers are UnboundVariables
        }
        else{
            Value*  return_val = env.getValue(*this);
            return return_val;
        }
    }

    friend bool operator<(const Identifier& lhs, const Identifier& rhs){
        return lhs.name < rhs.name;
    }

    virtual bool isValidPattern(std::set<std::string>& variables_occuring) override
    {
        if(std::isupper(name[0]) || name == "_") return true;
        else{
            if(variables_occuring.find(name) != variables_occuring.end()) throw std::runtime_error("variable occuring twice inside of pattern: " + name);
            else{
                variables_occuring.insert(name);
                return true;
            }
        }
    }

    virtual bool isIdentifier() override{return true;}

    virtual int getValueClassID() override{return IDENTIFIER_CLASS_ID;}
};

class Let : public Statement{
public:
    Let(Expression* pattern, Expression* expression, bool recursive): pattern(pattern), expression(expression), recursive(recursive){}

    Expression* pattern;
    Expression* expression;
    Value*      pattern_value;
    bool recursive;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + (recursive?std::string("Let rec: \n"):std::string("Let: \n")) + pattern->print(indents+1) + expression->print(indents+1);}
    virtual Type deduceType(Environment& env, Type mostGeneralExpected) override {
        if(pattern->exp_type.type_enum == UNDETERMINED) pattern->exp_type = env.getNewPolymorphicType();
        if(expression->exp_type.type_enum == UNDETERMINED) expression->exp_type = env.getNewPolymorphicType();

        std::set<std::string> pattern_parameters;
        pattern->isValidPattern(pattern_parameters);

        env.addRelation(pattern->exp_type, expression->exp_type);

        if(recursive){
            if(expression->isValue() && pattern->isIdentifier()){
                env.addIdentifierToBeTypeDeduced(*(Identifier*)pattern, false, pattern->exp_type);
                expression->deduceType(env, mostGeneralExpected);
                env.setIdentifierType(*(Identifier*)pattern, env.followRelations(pattern->exp_type));
            }
            else throw std::runtime_error("using rec keyword on non value or using rec on non identifier");
        }
        else{
            expression->deduceType(env, mostGeneralExpected); // deduce before adding it to identifier list, so it wont be visible recursively
            for(std::string param : pattern_parameters){
                env.addIdentifierToBeTypeDeduced(Identifier(param), false, env.getNewPolymorphicType());
            }
            env.execution_inside_pattern = true;
            pattern->deduceType(env, pattern->exp_type);
            env.execution_inside_pattern = false;
        }

        // mark that it is executed inside a pattern so Identifiers will be treated as UnboundVariables instead of trying to resolve them in the Environment, which will cause an error
        env.execution_inside_pattern = true;
        pattern_value = pattern->execute(env);
        env.execution_inside_pattern = false;

        return env.followRelations(pattern->exp_type);
    }

    virtual Value* execute(Environment& env) override;
};

class LetIn : public Expression{
public:
    LetIn(Let* let_statement, Expression* in_expression)
        :let_statement(let_statement), in_expression(in_expression){}

    Let* let_statement;
    Expression* in_expression;

    virtual std::string print(int indents) override {return let_statement->print(indents+1) + in_expression->print(indents+1);}
    virtual Type deduceType(Environment& env, Type mostGeneralExpected) override {
        Expression::deduceType(env, mostGeneralExpected);
        if(in_expression->exp_type.type_enum == UNDETERMINED) in_expression->exp_type = env.getNewPolymorphicType();

        env.addRelation(exp_type, in_expression->exp_type);

        env.addActivationFrame();
        let_statement->deduceType(env, env.getNewPolymorphicType());
        in_expression->deduceType(env, env.getNewPolymorphicType()); // with new variable 'letted', deduce type of in_expression
        env.removeActivationFrame();

        return exp_type = env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override {
        env.addActivationFrame();
        let_statement->execute(env);
        Value* return_val = in_expression->execute(env);
        env.removeActivationFrame();
        return return_val;
    } // execute
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

        return exp_type = env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override;
};

class FunctionCall : public Expression{
public:
    FunctionCall(Expression* function_expression, Expression* argument_expression): function_expression(function_expression), argument_expression(argument_expression)
    {isValueConstructorCall = function_expression->isIdentifier() && std::isupper(((Identifier*)function_expression)->name[0]); }

    Expression* function_expression;
    Expression* argument_expression;
    bool isValueConstructorCall;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Function Call: \n") + function_expression->print(indents+1) + argument_expression->print(indents+1);}

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        // only request new polymorphic type if needed
        ///if(function_expression->exp_type.type_enum == FUNCTION_TYPE) function_expression->deduceType(env, Type(FUNCTION_TYPE,"","",std::vector<Type>{Type(), mostGeneralExpected}));
        ///else function_expression->deduceType(env, Type(FUNCTION_TYPE,"","",std::vector<Type>{env.getNewPolymorphicType(), env.getNewPolymorphicType().getMoreSpecific(mostGeneralExpected)}));
        Expression::deduceType(env, mostGeneralExpected);

        Type function_expression_type = function_expression->deduceType(env, Type(FUNCTION_TYPE,"",std::vector<Type>{env.getNewPolymorphicType(), mostGeneralExpected}));
        Type argument_expression_type = argument_expression->deduceType(env, function_expression->exp_type.type_parameters[0]);

        env.addFunctionCallRelations(function_expression_type, argument_expression_type, exp_type);

        return exp_type = env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override {
        Value* function_val = function_expression->execute(env);
        Value* return_val = function_val->call(env, argument_expression->execute(env));;
        //if(isValueConstructorCall) return_val->exp_type = this->exp_type; // value constructors don't have time to determine return types inside
        return return_val;
    }

    virtual bool isValidPattern(std::set<std::string>& variables_occuring) override
    {
        if(function_expression->isIdentifier()){
            if(std::isupper(((Identifier*)function_expression)->name[0]) == false) throw std::runtime_error("calling not ValueConstructor function in pattern");
        }
        return argument_expression->isValidPattern(variables_occuring) && function_expression->isValidPattern(variables_occuring);
    }
};

class TupleCreation : public Expression{
public:
    TupleCreation(std::vector<Expression*>* tuple_elements): tuple_elements(tuple_elements) {}

    std::vector<Expression*>* tuple_elements;

    virtual std::string print(int indents) override {
        std::string result = std::string(indents, ' ') + std::string("Tuple Creation:\n");
        for(Expression* exp : *tuple_elements){
            result += exp->print(indents + 1);
        }
        return result;
    }

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        Expression::deduceType(env, mostGeneralExpected);
        Type tuple_type(COMPLEX, std::to_string(tuple_elements->size()) + "tuple" );

        tuple_type.type_parameters.resize(tuple_elements->size());
        for(unsigned int i = 0; i < tuple_elements->size(); ++i){
            tuple_type.type_parameters[i] = env.getNewPolymorphicType();
            tuple_elements->operator [](i)->deduceType(env, tuple_type.type_parameters[i]);
        }

        env.addRelation(exp_type, tuple_type);

        return exp_type = env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override;

    virtual bool isValidPattern(std::set<std::string>& variables_occuring) override
    {
        bool ok = true;
        for(Expression* elem : *tuple_elements){
            ok = ok | elem->isValidPattern(variables_occuring);
        }
        return ok;
    }
};

class MatchWith : public Expression{
public:
    MatchWith(Expression* matched_expression, std::vector< std::pair<Expression*, Expression*> > patterns_and_cases_expressions)
        :matched_expression(matched_expression), patterns_and_cases_expressions(patterns_and_cases_expressions){ patterns_values.resize(patterns_and_cases_expressions.size()); }

    Expression* matched_expression;
    std::vector< std::pair<Expression*, Expression*> > patterns_and_cases_expressions;
    std::vector<Value*>      patterns_values;

    virtual std::string print(int indents) override {
        std::string result = std::string(indents, ' ') + std::string("Match ") + matched_expression->print(0) + " with:";
        for(auto pat_cas : patterns_and_cases_expressions){
            result += pat_cas.first->print(indents + 1) + "   --> " + pat_cas.second->print(indents + 1);
        }
        return result;
    }
    virtual Type deduceType(Environment& env, Type mostGeneralExpected) override {
        Expression::deduceType(env, mostGeneralExpected);
        if(matched_expression->exp_type.type_enum == UNDETERMINED) matched_expression->exp_type = env.getNewPolymorphicType();
        matched_expression->deduceType(env, matched_expression->exp_type);
        int i = 0;
        for(auto pat_cas : patterns_and_cases_expressions){
            Expression* pattern = pat_cas.first;
            Expression* in_case = pat_cas.second;
            if(pattern->exp_type.type_enum == UNDETERMINED) pattern->exp_type = env.getNewPolymorphicType();
            if(in_case->exp_type.type_enum == UNDETERMINED) in_case->exp_type = env.getNewPolymorphicType();
            env.addRelation(pattern->exp_type, matched_expression->exp_type); // all patterns have to be of the same type as matched expression
            env.addRelation(in_case->exp_type, this->exp_type); // all cases have to have the same type as the whole match

            std::set<std::string> pattern_parameters;
            pattern->isValidPattern(pattern_parameters);

            env.addActivationFrame();
            for(std::string param : pattern_parameters){
                env.addIdentifierToBeTypeDeduced(Identifier(param), false, env.getNewPolymorphicType());
            }
            env.execution_inside_pattern = true;
            pattern->deduceType(env, pattern->exp_type);
            patterns_values[i++] = pattern->execute(env);
            env.execution_inside_pattern = false;

            in_case->deduceType(env, in_case->exp_type);

            env.removeActivationFrame();
        } // for every pattern expression pair

        return exp_type = env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override {
        env.addActivationFrame();

        Value* expression_result = matched_expression->execute(env);
        Expression* choosen_case = nullptr;
        Value* choosen_pattern_value = nullptr;
        int i = 0;
        bool matched = false;
        for(auto pat_cas : patterns_and_cases_expressions){
            Value* pattern_value = patterns_values[i++];
            Expression* in_case = pat_cas.second;
            if(pattern_value->matchWithValue(expression_result)){
                choosen_pattern_value = pattern_value;
                matched = true;
                choosen_case  = in_case;
                break;
            }
        } // for each pattern and case

        if(matched){
            choosen_pattern_value->applyMatch(expression_result, env);
            Value* return_val = choosen_case->execute(env);
            env.removeActivationFrame();
            return return_val;
        }
        else throw std::runtime_error("Failed to match!");
    } // execute
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
        env_copy.addActivationFrame();
        env_copy.addValue(*arg_name, argument);
        Value* return_value = function_expression->execute(env_copy);
        env_copy.removeActivationFrame();
        return return_value;
    }

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Function: \n") + arg_name->print(indents+1) + function_expression->print(indents+1);}
    virtual std::string printValue() override { return exp_type.type_name; }

    virtual Type deduceType(Environment &env, Type mostGeneralExpected) override {
        env.addActivationFrame();
            if(exp_type.type_enum == UNDETERMINED) exp_type = Type(FUNCTION_TYPE, "", std::vector<Type>{env.getNewPolymorphicType(), env.getNewPolymorphicType()});
            if(env.followRelations(exp_type).type_enum == POLYMORPHIC){
                env.addRelation(exp_type, Type(FUNCTION_TYPE, "", std::vector<Type>{env.getNewPolymorphicType(), env.getNewPolymorphicType()}));
                exp_type = env.followRelations(exp_type);
            }
            env.addRelation(exp_type, mostGeneralExpected);
            env.addIdentifierToBeTypeDeduced(*arg_name, false, exp_type.type_parameters[0]);
            function_expression->deduceType(env, exp_type.type_parameters[1]);
        env.removeActivationFrame();
        return exp_type = env.followRelations(exp_type);
    }

    virtual Value* execute(Environment& env) override {
        Function* function_copy = new Function(*this);
        function_copy->env_copy = env;
        return function_copy;
    }

    virtual int getValueClassID() override{return FUNCTION_CLASS_ID;}
    virtual bool isNonBuiltInFunction() override { return true; }
};


class BuiltIn_Function : public Value{
public:
    BuiltIn_Function(std::function<Value*(Value*)> fun, Type argument_type, Type return_type): fun(fun) { exp_type = Type(FUNCTION_TYPE,"",std::vector<Type>{argument_type, return_type});}
    std::function<Value*(Value*)> fun;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("BuiltIn Function\n");}
    virtual std::string printValue() override { return exp_type.type_name; }

    virtual Value* call(Environment& env, Value* argument) override {
        return fun.operator()(argument);
    }

    virtual Value* execute(Environment& env) override {
        return this;
    }

    virtual int getValueClassID() override{return BUILT_IN_FUNCTION_CLASS_ID;}
};

class ComplexValue : public Value{
public:
    ComplexValue(Type type, std::string constructor_name, std::vector<Value*> aggregatedValues = std::vector<Value*>())
        :constructor_name(constructor_name)
        ,aggregatedValues(aggregatedValues)
        { Expression::exp_type = type; }

    //Type type;
    std::string constructor_name;
    std::vector<Value*> aggregatedValues;

    virtual std::string print(int indents) override {
        std::stringstream ss;
        ss << std::string(indents, ' ') << constructor_name << "(";
        for(unsigned int i=0; i<aggregatedValues.size(); ++i) ss << aggregatedValues[i]->print(indents + 1);
        ss << ")\n";
        return ss.str();
    }

    virtual std::string printValue() override {
        std::stringstream ss;
        if(exp_type.type_name == "list"){
            ComplexValue* current = this;
            int i = 0;
            ss << "[";
            while(current->constructor_name == "Elem"){
                ss << ((ComplexValue*)current->aggregatedValues[0])->aggregatedValues[0]->printValue();
                current = (ComplexValue*)((ComplexValue*)current->aggregatedValues[0])->aggregatedValues[1];
                if(current->constructor_name == "Elem") ss << ";" << ((++i)%LIST_LINE_BREAK==0?"\n":"");
            }
            ss << "]";
        }
        else{
            if(std::isdigit(constructor_name[0]) == false ){ // if not displaying tuple type
                ss << constructor_name;
            }
            ss << "(";
            for(unsigned int i=0; i<aggregatedValues.size(); ++i) ss << aggregatedValues[i]->printValue() << (i==aggregatedValues.size()-1?"":", ");
            ss << ")";
        }
        return ss.str();
    }

    virtual bool isValidPattern(std::set<std::string>& variables_occuring) override
    {
        bool ok = true;
        for(Value* val : aggregatedValues){
            ok = ok | val->isValidPattern(variables_occuring);
        }
        return ok;
    }

    virtual int getValueClassID() override{return COMPLEX_VALUE_CLASS_ID;}
};

class Primitive : public Value{
public:

    virtual int getValueClassID() override{return PRIMITIVE_CLASS_ID;}
};

class Integer : public Primitive{
public:

    Integer(int value):value(value){exp_type = Type(PRIMITIVE, "int");}

    int value;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Integer: ") + std::to_string(value) + "\n";}
    virtual std::string printValue() override { return std::to_string(value); }

    virtual int getValueClassID() override{return PRIMITIVE_CLASS_ID;}
};

class Float : public Primitive{
public:
    Float(double value): value(value){exp_type = Type(PRIMITIVE, "float");}

    double value;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Float: ") + std::to_string(value) + "\n";}
    virtual std::string printValue() override { return std::to_string(value); }
};

class Bool : public Primitive{
public:
    Bool(bool value): value(value){exp_type = Type(PRIMITIVE, "bool");}

    bool value;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("Bool: ") + (value?"true":"false") + "\n";}
    virtual std::string printValue() override { return std::string(value?"true":"false"); }
};

class String : public Primitive{
public:
    String(std::string value): value(value){exp_type = Type(PRIMITIVE, "string");}

    std::string value;

    virtual std::string print(int indents) override {return std::string(indents, ' ') + std::string("String: \"") + value + "\"\n";}
    virtual std::string printValue() override { return std::string("\"") + value + std::string("\""); }
};

#endif // AST_H

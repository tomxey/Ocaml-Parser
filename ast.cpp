#include "ast.h"


Value *Conditional::execute(Environment &env)
{
    Value* return_val;
    if( ((Bool*)condition->execute(env))->value == true ){
        return_val = true_path->execute(env);
    }
    else{
        return_val = false_path->execute(env);
    }
    return return_val;
}


bool Value::equals(Value *other)
{
    return this->smallerThan(other) == other->smallerThan(this);
    /*int comparedClasses = this->getValueClassID();
    if( comparedClasses == other->getValueClassID() ){
        // class specific checks
        if( comparedClasses == UNBOUND_VALUE_VARIABLE_CLASS_ID){
            throw std::runtime_error("UnboundPatternVariable values shouldn't be compared at runtime!");
        }
        else if( comparedClasses == IDENTIFIER_CLASS_ID){
            throw std::runtime_error("Identifier values shouldn't be compared at runtime!");
        }
        else if( comparedClasses == FUNCTION_CLASS_ID || comparedClasses == BUILT_IN_FUNCTION_CLASS_ID){
            throw std::runtime_error("Function values shouldn't be compared at runtime!");
        }
        else if( comparedClasses == COMPLEX_VALUE_CLASS_ID){
            if( ((ComplexValue*)this)->constructor_name != ((ComplexValue*)other)->constructor_name) return false;
            for(unsigned int i = 0; i < ((ComplexValue*)this)->aggregatedValues.size() ; ++i){
                if( ((ComplexValue*)this)->aggregatedValues[i]->equals(((ComplexValue*)other)->aggregatedValues[i]) == false ) return false;
            }
            return true;
        }
        else if( comparedClasses == PRIMITIVE_CLASS_ID){
            if(exp_type.type_name == other->exp_type.type_name){
                if(exp_type.type_name == "int"){
                    return ((Integer*)this)->value == ((Integer*)other)->value;
                }
                else if(exp_type.type_name == "float"){
                    return ((Float*)this)->value == ((Float*)other)->value;
                }
                else if(exp_type.type_name == "bool"){
                    return ((Bool*)this)->value == ((Bool*)other)->value;
                }
                else if(exp_type.type_name == "string"){
                    return ((String*)this)->value == ((String*)other)->value;
                }
                else throw std::runtime_error("comparing unknown Primitive Value Classes");
            }
            else return false;
        }
        else throw std::runtime_error("comparing unknown Value Classes!");
    }
    else throw std::runtime_error("comparing values of different Classes");*/
} // equals

bool Value::smallerThan(Value *other)
{
    int comparedClasses = this->getValueClassID();
    if( comparedClasses == other->getValueClassID() ){
        if( comparedClasses == COMPLEX_VALUE_CLASS_ID){
            if( ((ComplexValue*)this)->constructor_name < ((ComplexValue*)other)->constructor_name) return true;
            else if( ((ComplexValue*)this)->constructor_name > ((ComplexValue*)other)->constructor_name) return false;
            else{
                for(unsigned int i = 0; i < ((ComplexValue*)this)->aggregatedValues.size() ; ++i){
                    if( ((ComplexValue*)this)->aggregatedValues[i]->smallerThan(((ComplexValue*)other)->aggregatedValues[i])) return true;
                    if( ((ComplexValue*)other)->aggregatedValues[i]->smallerThan(((ComplexValue*)this)->aggregatedValues[i])) return false;
                }
                return false;
            }
        }
        else if( comparedClasses == PRIMITIVE_CLASS_ID){
            if(exp_type.type_name == other->exp_type.type_name){
                if(exp_type.type_name == "int"){
                    return ((Integer*)this)->value < ((Integer*)other)->value;
                }
                else if(exp_type.type_name == "float"){
                    return ((Float*)this)->value < ((Float*)other)->value;
                }
                else if(exp_type.type_name == "bool"){
                    return ((Bool*)this)->value < ((Bool*)other)->value;
                }
                else if(exp_type.type_name == "string"){
                    return ((String*)this)->value < ((String*)other)->value;
                }
                else throw std::runtime_error("comparing unknown Primitive Value Classes");
            }
            else throw std::runtime_error("comparing primitives of different types: " + exp_type.type_name + " and " + other->exp_type.type_name);
        }
        else throw std::runtime_error("comparing uncomparable Value Classes!");
    }
    else throw std::runtime_error("comparing values of different Classes");
}

bool Value::matchWithValue(Value *other)
{
    if(this->getValueClassID() == UNBOUND_VALUE_VARIABLE_CLASS_ID) return true; // unbound variable matches to anything
    else{
        int comparedClasses = this->getValueClassID();
        if( comparedClasses == other->getValueClassID() ){
            if( comparedClasses == COMPLEX_VALUE_CLASS_ID){
                if( ((ComplexValue*)this)->constructor_name != ((ComplexValue*)other)->constructor_name) return false;
                for(unsigned int i = 0; i < ((ComplexValue*)this)->aggregatedValues.size() ; ++i){
                    if( ((ComplexValue*)this)->aggregatedValues[i]->matchWithValue(((ComplexValue*)other)->aggregatedValues[i]) == false ) return false;
                }
                return true;
            }
            else if( comparedClasses == PRIMITIVE_CLASS_ID){
                return this->equals(other);
            }
            else throw std::runtime_error("comparing unknown Value Classes!");
        }
        else return false;
    }
} // match with value

void Value::applyMatch(Value *other, Environment &env)
{
    if(this->getValueClassID() == UNBOUND_VALUE_VARIABLE_CLASS_ID){
        if( ((UnboundPatternVariable*)this)->name != "_" ) env.addValue(Identifier( ((UnboundPatternVariable*)this)->name ), other);
    }
    else{
        int comparedClasses = this->getValueClassID();
        if( comparedClasses == COMPLEX_VALUE_CLASS_ID){
            for(unsigned int i = 0; i < ((ComplexValue*)this)->aggregatedValues.size() ; ++i){
                ((ComplexValue*)this)->aggregatedValues[i]->applyMatch( ((ComplexValue*)other)->aggregatedValues[i], env);
            }
        }
    }
} // match with value


Value *TupleCreation::execute(Environment &env)
{
    ComplexValue* return_val = new ComplexValue(exp_type, std::to_string(tuple_elements->size()) + "Tuple");
    for(unsigned int i = 0; i < tuple_elements->size(); ++i){
        return_val->aggregatedValues.push_back( tuple_elements->operator [](i)->execute(env) );
    }
    return return_val;
}


Value *Let::execute(Environment &env)
{
    if(recursive){
        if(expression->isNonBuiltInFunction() && pattern->isIdentifier()){
            Function* function_copy = new Function(*(Function*)expression);
            env.addValue(*(Identifier*)pattern, static_cast<Value*>(function_copy));
            function_copy->env_copy = env;
            // im not executing function_copy as it will return another copy
            return nullptr;
        }
        else throw std::runtime_error("rec keyword can only be used to declare functions");
    }
    else{
        Value* expression_result = expression->execute(env);
        if(pattern_value->matchWithValue(expression_result)) pattern_value->applyMatch(expression_result, env);
        else throw std::runtime_error("Failed to match!");
        return nullptr;
    }
} // Let::execute


Type TypeDefsAST::deduceType(Environment &env, Type)
{
    env.addTypes(typeDefs);
    return env.getType(Identifier(typeDefs[0]->type_name));
}


Type TypeDefAST::deduceType(Environment &env, Type)
{
    env.addType(this);
    return env.getType(Identifier(this->type_name));
}

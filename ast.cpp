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
    int comparedClasses = this->getValueClassID();
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
    else return false;
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

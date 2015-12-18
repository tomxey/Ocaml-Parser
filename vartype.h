#ifndef VARTYPE_H
#define VARTYPE_H

#include <vector>
#include <string>
#include <map>
#include <ostream>
//#include "ast.h"

#define MAX_POLYMORPHIC_ID 1000000

enum TypeEnum{
    UNDETERMINED,
    POLYMORPHIC,
    COMPLEX,
    FUNCTION_TYPE,
    PRIMITIVE
};

class Type{
public:
    int polymorphic_helper_id;

    Type(TypeEnum type_enum = UNDETERMINED, std::string type_name = "", std::string constructor_name = "", std::vector<Type> aggregated_types = std::vector<Type>())
        : polymorphic_helper_id(MAX_POLYMORPHIC_ID), type_enum(type_enum), type_name(type_name), constructor_name(constructor_name), aggregated_types(aggregated_types){}

    Type withExpected(Type expected_type);
    Type getMoreGeneral(Type other_type);
    Type getMoreSpecific(Type other_type);
    bool relatedWith(Type other_type);

    Type withTypeSwapped(Type fromType, Type toType);

    void getTypeMapping(std::map<Type, Type> &mapping, Type to);
    Type withAppliedMapping(const std::map<Type, Type> &mapping);
    Type withArgumentApplied(Type argumentType);

    TypeEnum type_enum;
    std::string type_name;
    std::string constructor_name;
    std::vector<Type> aggregated_types;

    friend std::ostream& operator<<(std::ostream& os, const Type& val){
        if(val.type_enum == UNDETERMINED) os << "UNDETERMINED";
        else if(val.type_enum == POLYMORPHIC) os << "POLYMORPHIC: " << val.type_name;
        else if(val.type_enum == COMPLEX){
            os << "COMPLEX: " << val.type_name << ' ' << val.constructor_name << ' ';
            for(unsigned int i=0; i<val.aggregated_types.size(); ++i) os << val.aggregated_types[i] << ' ';
        }
        else if(val.type_enum == FUNCTION_TYPE) os << "FUNCTION_TYPE: " << val.aggregated_types[0] << " -> " << val.aggregated_types[1];
        else os << "PRIMITIVE: " << val.type_name;

        return os;
    }

    bool operator==(const Type& other) const{
        if(type_enum == other.type_enum && type_name == other.type_name && constructor_name == other.constructor_name && aggregated_types == other.aggregated_types) return true;
        else return false;
    }

    bool operator!=(const Type& other) const{
        return !this->operator ==(other);
    }

    bool operator<(const Type& other) const{
        if(type_enum == other.type_enum ){
            if(type_name == other.type_name ){
                if(constructor_name == other.constructor_name ){
                    return aggregated_types < other.aggregated_types;
                }
                else return constructor_name < other.constructor_name;
            }
            else return type_name < other.type_name;
        }
        else return type_enum < other.type_enum;
    }
};

/*class Type
{
public:

};

class WeakPolymorphicType : public Type{

};

class PolymorphicType : public Type{

};

class ComplexType : public Type{

};

class FunctionType : public Type{

};

class PrimitiveType : public Type{

};
*/

/** TYPE DEF BEGIN **/

class TypeDef{
public:
    TypeDef(std::string type_name, std::vector<Type> constructors = std::vector<Type>())
        :type_name(type_name), constructors(constructors){}

    std::string type_name;
    std::vector<Type> constructors; // ComplexTypes containing constructor names along with argument types
};

#endif // VARTYPE_H

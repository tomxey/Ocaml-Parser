#ifndef VARTYPE_H
#define VARTYPE_H

#include <vector>
#include <string>
#include <map>
//#include "ast.h"

enum TypeEnum{
    UNDETERMINED,
    POLYMORPHIC,
    COMPLEX,
    FUNCTION_TYPE,
    PRIMITIVE
};

class Type{
public:
    Type(TypeEnum type_enum = UNDETERMINED, std::string type_name = "", std::string constructor_name = "", std::vector<Type> aggregated_types = std::vector<Type>())
        : type_enum(type_enum), type_name(type_name), constructor_name(constructor_name), aggregated_types(aggregated_types){}

    Type withExpected(Type expected_type);
    Type getMoreGeneral(Type other_type);
    Type getMoreSpecific(Type other_type);
    bool relatedWith(Type other_type);

    TypeEnum type_enum;
    std::string type_name;
    std::string constructor_name;
    std::vector<Type> aggregated_types;
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

#ifndef VARTYPE_H
#define VARTYPE_H

#include <vector>
#include <string>
#include <map>
//#include "ast.h"

class Type
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

/** TYPE DEF BEGIN **/

class TypeDef{
public:
    TypeDef(std::string type_name, std::vector<ComplexType> constructors = std::vector<ComplexType>())
        :type_name(type_name), constructors(constructors){}

    std::string type_name;
    std::vector<ComplexType> constructors; // ComplexTypes containing constructor names along with argument types
};

#endif // VARTYPE_H

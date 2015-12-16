#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <deque>
#include <algorithm>
#include <vector>
#include "value.h"
#include "vartype.h"

class Identifier;
class Value;

class Environment
{
public:
    Environment():polymorphic_types_in_statement(0){}

    void addActivationFrame();
    void removeActivationFrame();

    void addIdentifierToBeTypeDeduced(Identifier identifier, bool rec = false, Type startingType = Type());
    void setIdentifierType(Identifier identifier, Type newType);
    Type getIdentifierType(Identifier identifier);

    Type get_new_polymorphic_type();
    void reset_polymorphic_types();
    int polymorphic_types_in_statement;

    void addValue(Identifier identifier, Value* value);
    Value* getValue(Identifier identifier);

    //void addFunctionArgument(Value* value);
    //Value* acceptFunctionArgument();

    std::deque< std::pair<Identifier, Type> > identifier_types;
    //std::deque< std::pair<Identifier, Type> > identifier_types_rec;

    // pointers used so values stored could be polymorphic
    std::deque< std::pair<Identifier, Value*> > env;
    std::deque< std::pair<Identifier, TypeDef> > type_defs;
};

#include "ast.h"

#endif // ENVIRONMENT_H

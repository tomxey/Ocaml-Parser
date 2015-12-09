#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include "value.h"
#include "ast.h"

class Identifier;
class Value;
class Type;
class TypeDef;

class Environment
{
public:


    // pointers used so values stored could be polymorphic
    std::map<Identifier, Value*> env;
    std::map<Identifier, Value*> rec_env;
    std::map<Identifier, Type*> unbound_vars;
    std::map<Identifier, TypeDef> type_defs;
};

#endif // ENVIRONMENT_H

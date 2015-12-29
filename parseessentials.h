#ifndef PARSEESSENTIALS_H
#define PARSEESSENTIALS_H

#include <vector>
#include "environment.h"
#include "ast.h"

class Statement;
class Environment;

#define DEFINE_CURRIED_FUNCTION(name, arga, argb, body, typea, typeb, typeto) ParseEssentials::toplevel_environment.addValue(Identifier(name), new BuiltIn_Function([](Value* arga)->Value* { \
auto partially_applied = new BuiltIn_Function([arga](Value* argb)->Value*{ body },typeb, typeto); \
return partially_applied; \
}, typea, Type(FUNCTION_TYPE,"","",std::vector<Type>{typeb, typeto}) )); \


namespace ParseEssentials
{
    extern void parseNewStatements();
    extern void parseStatement(Statement* statement);

    extern std::vector<Statement*> toplevel_statements;
    extern Environment toplevel_environment;
}

#endif // PARSEESSENTIALS_H

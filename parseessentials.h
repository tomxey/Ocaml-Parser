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

#define COLOR_OUTPUT
//#undef COLOR_OUTPUT

#define RESET_CODE 0

#define BLACK_CODE 30
#define RED_CODE 31
#define GREEN_CODE 32
#define YELLOW_CODE 33
#define BLUE_CODE 34
#define MAGENTA_CODE 35
#define CYAN_CODE 36
#define WHITE_CODE 37

namespace ParseEssentials
{
    extern void parseNewStatements();
    extern void parseStatement(Statement* statement);

    extern std::vector<Statement*> toplevel_statements;
    extern Environment toplevel_environment;

    extern std::string getColorCode(int color_id, bool bright = true, bool bold = false, bool background = false);
}

#endif // PARSEESSENTIALS_H

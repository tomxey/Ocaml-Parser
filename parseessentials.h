#ifndef PARSEESSENTIALS_H
#define PARSEESSENTIALS_H

#include <string>
#include "ast.h"

using namespace std;

#define FUN_NAME_PLUS "PLUS"
#define FUN_NAME_MINUS "MINUS"
#define FUN_NAME_MULT "MULT"
#define FUN_NAME_DIV "DIVIDE"
#define FUN_NAME_EQUALS "EQUALS"
#define FUN_NAME_PRINT_INT "print_int"

namespace ParseEssentials
{
    string get_CPP_headers();
    string get_CPP_main_start();
    string get_CPP_main_end();
    string get_and_register_CPP_curried_function_for_infix_operator(string funName, string op, string lArgType, string rArgType, string toType);
    string get_and_register_CPP_function(string funName, string argName, string fromType, string toType, string functionBody);

    extern AST* treeRoot;
}

#endif // PARSEESSENTIALS_H

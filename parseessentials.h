#ifndef PARSEESSENTIALS_H
#define PARSEESSENTIALS_H

#include <vector>
#include "environment.h"
#include "ast.h"

class Statement;
class Environment;

namespace ParseEssentials
{
    extern std::vector<Statement*> toplevel_statements;
    extern Environment toplevel_environment;
}

#endif // PARSEESSENTIALS_H

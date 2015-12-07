#ifndef AST_H
#define AST_H
/* Abstract Syntax Tree */

#include <string>
#include <ostream>
#include <iostream>
#include "vartype.h"
#include "parseessentials.h"

using namespace std;

class VarType;

enum SyntaxElement{
    VariableDeclaration,
    FunctionLiteral,
    FunctionCall,
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    Conditional,
    ConsecutiveStatements
};

class AST
{
public:
    AST(SyntaxElement syntaxElement, AST* identifierTree, AST* argument, bool recursive = false);
    AST(AST* conditionTree, AST* firstPath, AST* secondPath);
    AST(AST* firstStatement, AST* secondStatement);
    AST(int integerLiteral);
    AST(double floatLiteral);
    AST(SyntaxElement syntaxElement, string ident_or_string);
    ~AST();

    void init();

    SyntaxElement syntaxElement;

    string name; // declared variable name, called function name, variable name, function literal argument name, identifier etc...
    AST* identifierTree; // or condition tree in case of "if"
    AST* argument; // function argument, variable declaration value... etc..
    AST* secondArgument; // needed for "if then else"
    bool recursive; // if variable declaration is recursive

    int integerLiteral; // integer literal, if any
    double floatLiteral;

    string printTree(int indents = 0) const;
    string toString() const;

    string parseTree(vector<string> recVariables = vector<string>());

    VarType type;
    VarType &determineType();

    bool isReturnable();
};

inline std::ostream& operator <<(std::ostream& out, const AST& other){
    cout << other.printTree() << endl;
    return out;
}

#endif // AST_H

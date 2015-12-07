#ifndef VARTYPE_H
#define VARTYPE_H

#include <vector>
#include <string>
#include <map>
//#include "ast.h"

using namespace std;

class AST;

class VarType
{
public:
    VarType(); // default constructor creates another wildcard
    VarType(string basicTypeName);
    VarType(string complexTypeName, string constructorName, vector<VarType> subTypes);

    bool conflicts; // true if "variable equation" conflict occured, eg. (int, int) dhould be (int, float) at the same time

    bool isWildcard; // true if it is 'a 'b etc...
    long long wildcardID;

    bool isBasicType; // true if it is int, float, string etc...
    string basicTypeName;

    bool isComplexType; // true if it is complex type, like 'a list, int list, (int, int float), int->(int,int)->int etc...
    string complexTypeName; // eg function, or list, or bt
    string constructorName; // Node, Empty, [], ::
    vector<VarType> subTypes; // subtypes for complex type
    //vector<AST*> subTypesSources; // subtypes sources, should be of the same length as subTypes
    // eg. type of if = type of arg1 or type of arg2

    // return false on apply failure
    bool applyType(VarType &); // in apply type, we try to convert ourselves to arg

    string toCPPtypeString();

// what if we deduce that 'a = ('d, int, float) , and later we deduce that 'c = 'b and later that 'b = 'a ??
// in this situation both 'c = 'a and 'b = 'a
// even worse... 'a can appear to be (float, int, float) and then we have to change both 'b and 'c....
// so we have to use some kind of links and sets... maybe

// another problem: determine when a given identifier points to some value(eg. int) and when it is a "wildcard" 'a


// here we make an important assumption: all variables in program have unique names!!
// identifiersTypes: each identifier has its' type specified in this array
// every existing(!) identifier has some starting type, every free variable has 'a type at the beginning
// variable not in identifiersTypes --> signal error
static map<string, VarType> identifiersTypes;
static long long numOfWildcards;
//static vector<VarType> expressionsTypes; // types of all expressions in AST, by ID
};

#endif // VARTYPE_H

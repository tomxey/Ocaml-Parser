#include "parseessentials.h"
#include "vartype.h"

AST* ParseEssentials::treeRoot;

string ParseEssentials::get_and_register_CPP_curried_function_for_infix_operator(string funName, string op, string lArgType, string rArgType, string toType)
{
    string result = "function< function<"+toType+"("+rArgType+")>("+lArgType+")> " + funName + " = []("+lArgType+" a)->function<"+toType+"("+rArgType+")>{\
        return [a]("+rArgType+" b)->"+toType+" { return a " + op + " b; };\
    };";
VarType::identifiersTypes[funName] = VarType("<fun>", "", vector<VarType>{
                                                 VarType(lArgType),
                                                 VarType("<fun>", "", vector<VarType>{VarType(rArgType), VarType(toType)}) });
return result;
}

string ParseEssentials::get_CPP_headers()
{
    string result = "#include <functional>\n\
        #include <string>\n";
    return result;
}

string ParseEssentials::get_CPP_main_start()
{
    string result = "int main(){";
    return result;
}

string ParseEssentials::get_CPP_main_end()
{
    string result = "}";
    return result;
}

string ParseEssentials::get_and_register_CPP_function(string funName, string argName, string fromType, string toType, string functionBody)
{
    string result = "function<"+toType+"("+fromType+")> " + funName + " = []("+fromType+" "+argName+")->"+toType+" { "+functionBody+" };";
VarType::identifiersTypes[funName] = VarType("<fun>", "", vector<VarType>{VarType(fromType), VarType(toType)});
return result;
}

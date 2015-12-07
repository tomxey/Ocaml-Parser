#include "ast.h"

AST::AST(SyntaxElement syntaxElement, AST *identifierTree, AST *argument, bool recursive)
{
    init();
    this->syntaxElement = syntaxElement;
    this->identifierTree = identifierTree;
    this->argument = argument;
    this->recursive = recursive;
}

AST::AST(AST *conditionTree, AST *firstPath, AST *secondPath)
{
    init();
    this->syntaxElement = Conditional;
    identifierTree = conditionTree; // dem names :/
    argument = firstPath;
    secondArgument = secondPath;
}

AST::AST(AST *firstStatement, AST *secondStatement)
{
    init();
    this->syntaxElement = ConsecutiveStatements;
    identifierTree = firstStatement;
    argument = secondStatement;
}

AST::AST(int integerLiteral)
{
    init();
    syntaxElement = IntegerLiteral;
    this->integerLiteral = integerLiteral;
}

AST::AST(double floatLiteral)
{
    init();
    this->syntaxElement = FloatLiteral;
    this->floatLiteral = floatLiteral;
}

AST::AST(SyntaxElement syntaxElement, string ident_or_string)
{
    init();
    this->syntaxElement = syntaxElement;
    name = ident_or_string;
}

AST::~AST()
{
    if(identifierTree != NULL) delete identifierTree;
    if(argument != NULL) delete argument;
    if(secondArgument != NULL) delete secondArgument;
}

void AST::init()
{
    this->syntaxElement = IntegerLiteral;
    this->name = "";
    this->identifierTree = NULL;
    this->argument = NULL;
    this->secondArgument = NULL;
    this->recursive = false;
    this->integerLiteral = 0;
    this->floatLiteral = 0;
}

string AST::printTree(int indents) const
{
    string result(indents, ' ');
    result += "├── " + toString() + "\n";
            if(identifierTree != NULL) result += identifierTree->printTree(indents + 1);
            if(argument != NULL) result += argument->printTree(indents + 1);
            if(secondArgument != NULL) result += secondArgument->printTree(indents + 1);

    return result;
}

string AST::toString() const
{
    string result;
    switch(syntaxElement){
        case VariableDeclaration: result = "Variable Declaration: "; break;
        case FunctionLiteral: result = "Function Literal: "; break;
        case FunctionCall: result = "Function Call: "; break;
        case Identifier: result = "Identifier: "; break;
        case IntegerLiteral: result = "Integer Literal: " + to_string(integerLiteral); break;
        case FloatLiteral: result = "Float Literal: " + to_string(floatLiteral); break;
        case StringLiteral: result = "String Literal: \"" + name + "\""; break;
        case Conditional: result = "Conditional: "; break;
        case ConsecutiveStatements: result = "Consecutive statements: "; break;
    }

    //if(syntaxElement != IntegerLiteral)
        result += (recursive?"Recursive " : "") + name;
    return result;
}

string AST::parseTree(vector<string> recVariables)
{
    string result;
    switch(syntaxElement){
        case VariableDeclaration:
        recVariables.push_back( identifierTree->parseTree() );
        result = argument->determineType().toCPPtypeString()+" " + identifierTree->parseTree() + " = " + argument->parseTree(recVariables) + ";"; break;
        case FunctionLiteral:
        {
            string closureString = "[=";
            for(unsigned int i = 0; i < recVariables.size(); ++i){
                closureString += ", &" + recVariables[i];
            }
            closureString += "]";
            result = "("+closureString+"("+ identifierTree->determineType().toCPPtypeString() +" "+ identifierTree->parseTree() +")->"+argument->determineType().toCPPtypeString()+
                    " { "+ (argument->isReturnable()?"return ":"") + argument->parseTree() +"; })" ; break;
        }
        case FunctionCall: result = identifierTree->parseTree() + "(" + argument->parseTree() + ")"; break;
        case Identifier: result = name; break;
        case IntegerLiteral: result = to_string(integerLiteral); break;
        case FloatLiteral: result = to_string(floatLiteral); break;
        case StringLiteral: result = "\"" + name + "\""; break;
        case Conditional: result = "if(" + identifierTree->parseTree() + "){ "+(argument->isReturnable()?"return ":"") + argument->parseTree() + "; }else{ " +
                (secondArgument->isReturnable()?"return ":"") + secondArgument->parseTree() + "; }"; break;
        case ConsecutiveStatements:
        if(argument == NULL){
            result = identifierTree->parseTree()+";";
        }
        else{
             result = identifierTree->parseTree()+";\n"+argument->parseTree(); break;
        }
        break;
    }

    return result;
}

VarType& AST::determineType()
{
    switch(syntaxElement){
        case VariableDeclaration:
        // if variable don't have type assigned yet... add as wildcard
        if(VarType::identifiersTypes.find(identifierTree->parseTree()) == VarType::identifiersTypes.end()){
            VarType::identifiersTypes[identifierTree->parseTree()] = VarType();
        }
            // we have the same type as argument, and variable = argument so....
            type.applyType( argument->determineType() );
            VarType::identifiersTypes[identifierTree->parseTree()].applyType(type);
        break;
        case FunctionLiteral:
        {
                VarType tmp = VarType("<fun>", "", vector<VarType>{ identifierTree->determineType(), argument->determineType() });
                // it shouldn't be like this I believe...
                type.applyType( tmp ); break;
        }
        case FunctionCall:
        {
                VarType bareFunction = VarType("<fun>", "", vector<VarType>{ VarType(), VarType() });
                VarType &functionType = identifierTree->determineType();
                functionType.applyType( bareFunction );
                if(functionType.isComplexType && functionType.complexTypeName == "<fun>"){
                    argument->determineType().applyType( functionType.subTypes[0] ); // type(argument) = typeOfFunctionFrom
                    type.applyType( functionType.subTypes[1] );  // type of function call == type of function return type
                }
                else{
                    //someerror
                }
        }
        break;
        case Identifier:  type.applyType( VarType::identifiersTypes[name] ); break;
        case IntegerLiteral:
        {
            VarType tmp("int");
            type.applyType( tmp ); break;
        }
        case FloatLiteral:
        {
            VarType tmp("double");
            type.applyType( tmp ); break;
        }
        case StringLiteral:
        {
            VarType tmp("std::string");
            type.applyType( tmp ); break;
        }
        case Conditional:
        {
            identifierTree->determineType();
            VarType &firstArg = argument->determineType();
            VarType &secondArg = secondArgument->determineType();
            firstArg.applyType(secondArg);
            type.applyType( firstArg ); break;
        }
        case ConsecutiveStatements:
            if(argument == NULL){
                type.applyType(identifierTree->determineType());
            }
            else{
                identifierTree->determineType(); type.applyType(argument->determineType());
            }
            break;
    }

    return type;
}

bool AST::isReturnable()
{
    if(syntaxElement == IntegerLiteral || syntaxElement == FunctionCall || syntaxElement == FunctionLiteral || syntaxElement == Identifier) return true;
    else return false;
}


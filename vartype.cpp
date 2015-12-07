#include "vartype.h"
#include <exception>

map<string, VarType> VarType::identifiersTypes;
//vector<VarType> VarType::expressionsTypes;
long long VarType::numOfWildcards = 0;

// defult constructor creates another wildcard.............
VarType::VarType()
{
    conflicts = false;
    isBasicType = isComplexType = false;
    isWildcard = true;
    wildcardID = VarType::numOfWildcards++;
}

VarType::VarType(string basicTypeName)
{
    conflicts = false;
    isWildcard = isComplexType = false;
    isBasicType = true;
    this->basicTypeName = basicTypeName;
}

VarType::VarType(string complexTypeName, string constructorName, vector<VarType> subTypes)
{
    conflicts = false;
    isWildcard = isBasicType = false;
    isComplexType = true;
    this->complexTypeName = complexTypeName;
    this->constructorName = constructorName;
    this->subTypes = subTypes;
}

bool VarType::applyType(VarType &other)
{
    if(other.conflicts || this->conflicts){
        this->conflicts = other.conflicts = true;
        throw std::runtime_error("VarType::applyType: Applying two arguments that are already conflicting");
        return false;
    }
    else if(this->isWildcard){
        this->isWildcard = other.isWildcard;
        this->wildcardID = other.wildcardID;

        this->isBasicType = other.isBasicType;
        this->basicTypeName = other.basicTypeName;

        this->isComplexType = other.isComplexType;
        this->complexTypeName = other.complexTypeName;
        this->constructorName = other.constructorName;
        this->subTypes = other.subTypes;

        return true;
    }
    else if(other.isWildcard){
        other.isWildcard = this->isWildcard;
        other.wildcardID = this->wildcardID;

        other.isBasicType = this->isBasicType;
        other.basicTypeName = this->basicTypeName;

        other.isComplexType = this->isComplexType;
        other.complexTypeName = this->complexTypeName;
        other.constructorName = this->constructorName;
        other.subTypes = this->subTypes;

        return true;
    }
    else if(this->isBasicType && other.isBasicType && this->basicTypeName == other.basicTypeName){
        return true;
    }
    else if(this->complexTypeName == other.complexTypeName && this->constructorName == other.constructorName && this->subTypes.size() == other.subTypes.size()){ // should constructors match too?
        // here we both should be complex types.... apply subtypes
        bool ok = true;
        for(unsigned int i = 0; i < this->subTypes.size() ; ++i){
            ok = ok && this->subTypes[i].applyType(other.subTypes[i]);
        }
        this->conflicts = other.conflicts = !ok;
        if(ok == false){
            throw std::runtime_error("VarType::applyType: Complex type subtypes doesn't match");
        }
        return ok;
    }
    else{
        this->conflicts = other.conflicts = true;
        throw std::runtime_error("VarType::applyType: Can't match types");
        return false;
    }
}

string VarType::toCPPtypeString()
{
    if(isWildcard){
        return "'" + to_string(wildcardID);
    }
    else if(isBasicType){
        return basicTypeName;
    }
    else{
        if(complexTypeName == "<fun>"){
            return "function<"+subTypes[1].toCPPtypeString()+"("+subTypes[0].toCPPtypeString()+")> ";
        }
        else{
            string result = complexTypeName + "(";
            for(unsigned int i = 0; i < subTypes.size(); ++i){
                result += subTypes[i].toCPPtypeString() + ((i == subTypes.size() - 1)? ")" : ",");
            } // for
            return result;
        } // else
    } // else
}

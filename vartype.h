#ifndef VARTYPE_H
#define VARTYPE_H

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <ostream>
//#include "ast.h"

//#define MAX_POLYMORPHIC_ID 1000000

enum TypeEnum{
    UNDETERMINED,
    POLYMORPHIC,
    COMPLEX,
    FUNCTION_TYPE,
    PRIMITIVE
};

class Type{
public:
    //int polymorphic_helper_id;

    Type(TypeEnum type_enum = UNDETERMINED, std::string type_name = "", std::vector<Type> type_parameters = std::vector<Type>())
        : type_enum(type_enum), type_name(type_name), type_parameters(type_parameters)
    { if(type_enum == FUNCTION_TYPE) type_name = "<fun>"; }

    //Type withExpected(Type expected_type);
    //Type getMoreGeneral(Type other_type);
    Type getMoreSpecific(Type other_type);
    bool relatedWith(Type other_type);

    Type withTypeSwapped(Type fromType, Type toType);

    void getTypeMapping(std::map<Type, Type> &mapping, Type to);
    Type withAppliedMapping(const std::map<Type, Type> &mapping);
    Type withArgumentApplied(Type argumentType);

    TypeEnum type_enum;
    std::string type_name;
    std::vector<Type> type_parameters;

    friend std::ostream& operator<<(std::ostream& os, const Type& val){
        if(val.type_enum == UNDETERMINED) os << "UNDETERMINED";
        else if(val.type_enum == POLYMORPHIC) os << "POLYMORPHIC: " << val.type_name;
        else if(val.type_enum == COMPLEX){
            os << "COMPLEX: " << val.type_name << ' ';
            for(unsigned int i=0; i<val.type_parameters.size(); ++i) os << val.type_parameters[i] << ' ';
        }
        else if(val.type_enum == FUNCTION_TYPE) os << "FUNCTION_TYPE: " << val.type_parameters[0] << " -> " << val.type_parameters[1];
        else os << "PRIMITIVE: " << val.type_name;

        return os;
    }

    std::string to_string(){
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }

    bool operator==(const Type& other) const{
        if(type_enum == other.type_enum && type_name == other.type_name && type_parameters == other.type_parameters) return true;
        else return false;
    }

    bool operator!=(const Type& other) const{
        return !this->operator ==(other);
    }

    bool operator<(const Type& other) const{
        if(type_enum == other.type_enum ){
            if(type_name == other.type_name ){
                    return type_parameters < other.type_parameters;
            }
            else return type_name < other.type_name;
        }
        else return type_enum < other.type_enum;
    }
};

/** TYPE DEF BEGIN **/

/*class TypeDef{
public:
    TypeDef(std::string type_name, std::vector<std::string> polymorphic_parameters_names, std::vector<Type> constructors = std::vector<Type>())
        :type_name(type_name), polymorphic_parameters_names(polymorphic_parameters_names), constructors(constructors){}

    std::string type_name;
    std::vector<std::string> polymorphic_parameters_names;
    std::vector<Type> type_parameters;

    //TODO: think about it
//type ('a,'b,....,'n) name = Cons1 of (t1,t2,..,tn) type_name | Cons2 | Cons3;;
    // tn = arg1,arg2,.... type_name
    //.. recursively
    std::vector<Type> constructors; // ComplexTypes containing constructor names along with argument types
};*/

#endif // VARTYPE_H

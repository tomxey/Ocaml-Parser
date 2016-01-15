#include "environment.h"
#include <set>

#define ACTIVATION_FRAME_IDENTIFIER "--afi--"

void Environment::addActivationFrame()
{
    identifier_types.push_back( std::make_pair(Identifier(ACTIVATION_FRAME_IDENTIFIER), Type()) );
    //identifier_types_rec.push_back( std::make_pair(Identifier(ACTIVATION_FRAME_IDENTIFIER), Type()) );

    identifiers_stack.push_back( Identifier(ACTIVATION_FRAME_IDENTIFIER));
}

void Environment::removeActivationFrame()
{
    while(!identifier_types.empty() && identifier_types.back().first.name != ACTIVATION_FRAME_IDENTIFIER) identifier_types.pop_back();
    //while(!identifier_types_rec.empty() && identifier_types_rec.back().first.name != ACTIVATION_FRAME_IDENTIFIER) identifier_types_rec.pop_back();
    while(!identifiers_stack.empty() && identifiers_stack.back().name != ACTIVATION_FRAME_IDENTIFIER){
        Identifier to_remove = identifiers_stack.back();

        identifiers_stack.pop_back();
        variables[to_remove].pop_back();

        if(variables[to_remove].size() == 0) variables.erase(variables.find(to_remove));
    }

    if(!identifier_types.empty()) identifier_types.pop_back();
    //if(!identifier_types_rec.empty()) identifier_types_rec.pop_back();
    if(!identifiers_stack.empty()) identifiers_stack.pop_back();
}

void Environment::addIdentifierToBeTypeDeduced(Identifier identifier, bool rec, Type startingType)
{
    //if(rec == false)
        identifier_types.push_back(std::make_pair(identifier, startingType));
    //else identifier_types_rec.push_back(std::make_pair(identifier, startingType));
}

void Environment::setIdentifierType(Identifier identifier, Type newType)
{
    std::list< std::pair<Identifier, Type> >::reverse_iterator iter;
    for(iter = identifier_types.rbegin(); iter != identifier_types.rend() && iter->first.name != identifier.name; ++iter);
    if(iter != identifier_types.rend()){
        std::pair<Identifier, Type> found = *iter;
        addRelation(iter->second, newType);
    }
    else{
        // it is not considered an arror anymore
        //throw std::runtime_error("identifier not found");
    }
}

Type Environment::getIdentifierType(Identifier identifier)
{
    for(auto iter = identifier_types.rbegin(); iter != identifier_types.rend(); ++iter){
        if(iter->first.name == identifier.name){
            Type toReturn = iter->second;
            return toReturn;
        }
    }

    if(variables.find(identifier) != variables.end()){
        return renumeratedToUnique( followRelations(variables[identifier].back()->exp_type) );
    }
    throw std::runtime_error("identifier "+ identifier.name +" not found");
}

Type Environment::getNewPolymorphicType()
{
    Type toReturn(POLYMORPHIC, "'" + std::to_string(polymorphic_types_in_statement++));
    return toReturn;
}

void Environment::reset_polymorphic_types()
{
    polymorphic_types_in_statement = 0;
}

void Environment::addValue(Identifier identifier, Value *value)
{
    identifiers_stack.push_back(identifier);
    variables[identifier].push_back(value);
}

Value *Environment::getValue(Identifier identifier)
{
    if(variables.find(identifier) != variables.end()){
        return variables[identifier].back();
    }
    throw std::runtime_error("value not found for identifier: " + identifier.name);
}

bool Environment::valueExists(Identifier identifier)
{
    return variables.find(identifier) != variables.end();
}

void Environment::printNewValues()
{
    auto iter = identifiers_stack.end();
    while(displayed_values < identifiers_stack.size()){
        --iter;
        ++displayed_values;
    }

    while(iter != identifiers_stack.end()){
        Identifier identifier = *(iter++);
        std::cout << identifier.name << ": " << renumeratedToSmallest(variables[identifier].back()->exp_type) << " = " << variables[identifier].back()->print(0);
        std::cout.flush();
    }
}

Type Environment::addType(TypeDefAST *type_def)
{
    std::vector<Type> type_parameters;
    std::set<std::string> parameters_set;
    for(std::string param : type_def->polymorphic_parameters_names){
        type_parameters.push_back(Type(POLYMORPHIC, param));
        parameters_set.insert(param);
        if(param[0] != '\'') throw std::runtime_error("bad polymorphic parameter name: " + param);
    }
    if(parameters_set.size() != type_parameters.size()) throw std::runtime_error("parameter name used twice in type definition");

    Type type_added(COMPLEX, type_def->type_name, type_parameters);

    if(type_constructors.find(Identifier(type_def->type_name)) == type_constructors.end()){
        type_constructors[Identifier(type_def->type_name)] = type_added;
    }
    else{
        throw std::runtime_error("type " + type_def->type_name + " already exists");
    }

    // check if all types in Value Constructors exist

    std::deque<Type*> Q;
    for(std::pair<std::string, Type>& pair : type_def->constructors){
        if(pair.second.type_enum != UNDETERMINED)  Q.push_back(&pair.second);
    }

    while(!Q.empty()){
        Type* current = Q.front(); Q.pop_front();
        if(current->type_enum == POLYMORPHIC){
            if(parameters_set.find(current->type_name) == parameters_set.end()) throw std::runtime_error("using nonexisting type: " + current->type_name);
        }
        else{
            Type type_constructed = getType(Identifier(current->type_name));
            if(type_constructed.type_parameters.size() == current->type_parameters.size()){
                for(Type& type : current->type_parameters){
                    Q.push_back(&type);
                }
            }
            else throw std::runtime_error("wrong number of parameters in type constructor: " + type_constructed.type_name);
        }
    } // while Q

    /// now it is time to add Value Constructors
    /// constructors wit argument will be functions, no-argument constructors will be ordinary variables
/**
    ParseEssentials::toplevel_environment.addValue(Identifier("fst"), // Constructor name
                                               new BuiltIn_Function([](Value* arg)->Value* //
                                                {return ((ComplexValue*)arg)->aggregatedValues[0];}, // return new complex value of type with specified constructor name, and argument as aggregatedValue
                                               Type(COMPLEX, "pair", "",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}), // argument type
                                               Type(POLYMORPHIC, "'a") )); // return complex type
**/

    for(std::pair<std::string, Type>& pair : type_def->constructors){
        if(valueExists(Identifier(pair.first))) throw std::runtime_error("Value " + pair.first + " already exists in the environment");

        if(pair.second.type_enum == UNDETERMINED){
            addValue(Identifier(pair.first), new ComplexValue( type_added , pair.first));
        }
        else{
            ParseEssentials::toplevel_environment.addValue(Identifier(pair.first), // Constructor name
                                                   new BuiltIn_Function([type_added,pair](Value* arg)->Value* //
                                                    {return new ComplexValue(type_added, pair.first, std::vector<Value*>{arg});}, // return new complex value of type with specified constructor name, and argument as aggregatedValue
                                                   pair.second, // argument type
                                                   type_added )); // return complex type
        }
    } // for

    return type_added;
} // addType

Type Environment::getType(Identifier identifier)
{
    if(type_constructors.find(identifier) != type_constructors.end()) return renumeratedToUnique(type_constructors[identifier]);
    else if(std::isdigit(identifier.name[0])){ // only tuples have digit as first character
        std::stringstream ss(identifier.name);
        int tuple_size;
        std::string tuple_name;
        ss >> tuple_size >> tuple_name;
        if(tuple_name != "tuple") throw std::runtime_error("strange tuple name: " + tuple_name);
        else{
            Type tuple_type(COMPLEX, identifier.name);
            for(unsigned int i = 0; i < tuple_size; ++i){
                tuple_type.type_parameters.push_back( Type(POLYMORPHIC, "\'" + std::to_string(i)) );
            }
            return tuple_type;
        }
    }
    else throw std::runtime_error("type " + identifier.name + " doesn't exist");
}

void Environment::cleanupAfterStatement()
{
    identifier_types.clear();
    //reset_polymorphic_types();
    //clearRelations();
}

Type Environment::followRelations(Type type, int depth)
{
    if(depth > type_relations.size()) throw std::runtime_error("circular relations?");

    while(type.type_enum == POLYMORPHIC && type_relations[type].type_enum != UNDETERMINED){
        type = type_relations[type];
        depth++;
    }

    if(type.type_parameters.size() > 0){
        for(unsigned int i=0; i<type.type_parameters.size(); ++i){
            type.type_parameters[i] = followRelations(type.type_parameters[i], depth);
        }
    }

    return type;
}

void Environment::addFunctionCallRelations(Type function_type, Type argument_applied, Type result_expected)
{
    function_type = followRelations(function_type);
    argument_applied = followRelations(argument_applied);
    result_expected = followRelations(result_expected);

    if(function_type.type_enum == FUNCTION_TYPE){
        addRelation(function_type.type_parameters[0], argument_applied);
        function_type = followRelations(function_type); // function type might have changed, update it
        addRelation(function_type.type_parameters[1], result_expected);
    }
    else throw std::runtime_error("adding function relations using a non-function");
}

void Environment::addRelation(Type from, Type to)
{
    assert(from.type_enum != UNDETERMINED && to.type_enum != UNDETERMINED);

    from = followRelations(from);
    to = followRelations(to);

    if(from.type_enum == POLYMORPHIC && to.type_enum == POLYMORPHIC && from.type_name == to.type_name) return; // do not create loops
    if(from.type_enum == POLYMORPHIC) type_relations[from] = to;
    else if(to.type_enum == POLYMORPHIC) type_relations[to] = from;
    else{
        if(from.type_name != to.type_name) throw std::runtime_error("adding relation between:" + from.to_string() + " and " + to.to_string());
        for(unsigned int i=0; i<from.type_parameters.size(); ++i){
            addRelation(from.type_parameters[i], to.type_parameters[i]);
        }
    }

    /// unnecessary part removed, it appears that complex types, function types and primitives can all be handled the same way
    /*else if(from.type_enum == PRIMITIVE && to.type_enum == PRIMITIVE){
        if(from!=to) throw std::runtime_error("adding relation between:" + from.type_name + " and " + to.type_name);
    }
    else if(from.type_enum == COMPLEX && to.type_enum == COMPLEX && from.type_parameters.size() == to.type_parameters.size() || from.type_enum == FUNCTION_TYPE && to.type_enum == FUNCTION_TYPE){*/
}

void Environment::clearRelations()
{
    type_relations.clear();
}

Type Environment::renumeratedToSmallest(Type type)
{
    Environment fake_env;
    type = followRelations(type);
    return fake_env.renumeratedToUnique(type);
}

Type Environment::doRenumerations(Type type, std::map<Type, Type> &renumerations)
{
    if(type.type_enum == POLYMORPHIC){
        if(renumerations.find(type) != renumerations.end()) return renumerations[type];
        else return renumerations[type] = getNewPolymorphicType();
    }
    else{
        for(unsigned int i = 0; i < type.type_parameters.size(); ++i){
            type.type_parameters[i] = doRenumerations(type.type_parameters[i], renumerations);
        }
        return type;
    }
}

Type Environment::renumeratedToUnique(Type type)
{
    //type = followRelations(type);
    std::map<Type, Type> renumerations;
    return doRenumerations(type, renumerations);
}

std::string Environment::relationsToString()
{
    std::string str;
    for(std::pair<Type, Type> kv : type_relations){
        str += kv.first.to_string() + " --> " + kv.second.to_string() + "\n";
    }
    return str;
}

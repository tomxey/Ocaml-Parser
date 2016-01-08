#include "environment.h"

#define ACTIVATION_FRAME_IDENTIFIER "--afi--"

void Environment::addActivationFrame()
{
    identifier_types.push_back( std::make_pair(Identifier(ACTIVATION_FRAME_IDENTIFIER), Type()) );
    //identifier_types_rec.push_back( std::make_pair(Identifier(ACTIVATION_FRAME_IDENTIFIER), Type()) );

    env.push_back( std::make_pair(Identifier(ACTIVATION_FRAME_IDENTIFIER), nullptr) );
}

void Environment::removeActivationFrame()
{
    while(!identifier_types.empty() && identifier_types.back().first.name != ACTIVATION_FRAME_IDENTIFIER) identifier_types.pop_back();
    //while(!identifier_types_rec.empty() && identifier_types_rec.back().first.name != ACTIVATION_FRAME_IDENTIFIER) identifier_types_rec.pop_back();
    while(!env.empty() && env.back().first.name != ACTIVATION_FRAME_IDENTIFIER) env.pop_back();

    if(!identifier_types.empty()) identifier_types.pop_back();
    //if(!identifier_types_rec.empty()) identifier_types_rec.pop_back();
    if(!env.empty()) env.pop_back();
}

void Environment::addIdentifierToBeTypeDeduced(Identifier identifier, bool rec, Type startingType)
{
    //if(rec == false)
        identifier_types.push_back(std::make_pair(identifier, startingType));
    //else identifier_types_rec.push_back(std::make_pair(identifier, startingType));
}

void Environment::setIdentifierType(Identifier identifier, Type newType)
{
    std::deque< std::pair<Identifier, Type> >::reverse_iterator iter;
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
    for(auto iter = env.rbegin(); iter != env.rend(); ++iter){
        if(iter->first.name == identifier.name) return iter->second->exp_type;
    }
    throw std::runtime_error("identifier not found");
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
    env.push_back(std::make_pair(identifier, value));
}

Value *Environment::getValue(Identifier identifier)
{
    for(auto iter = env.rbegin(); iter != env.rend(); ++iter){
        if(iter->first.name == identifier.name) return iter->second;
    }
    return nullptr;
}

Type Environment::followRelations(Type type, int depth)
{
    if(depth > type_relations.size()) throw std::runtime_error("circular relations?");

    while(type.type_enum == POLYMORPHIC && type_relations[type].type_enum != UNDETERMINED){
        type = type_relations[type];
    }

    if(type.aggregated_types.size() > 0){
        for(unsigned int i=0; i<type.aggregated_types.size(); ++i){
            type.aggregated_types[i] = followRelations(type.aggregated_types[i], depth + 1);
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
        addRelation(function_type.aggregated_types[0], argument_applied);
        function_type = followRelations(function_type); // function type might have changed, update it
        addRelation(function_type.aggregated_types[1], result_expected);
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
    else if(from.type_enum == PRIMITIVE && to.type_enum == PRIMITIVE){
        if(from!=to) throw std::runtime_error("adding relation between:" + from.type_name + " and " + to.type_name);
    }
    else if(from.type_enum == COMPLEX && to.type_enum == COMPLEX && from.aggregated_types.size() == to.aggregated_types.size() || from.type_enum == FUNCTION_TYPE && to.type_enum == FUNCTION_TYPE){
        for(unsigned int i=0; i<from.aggregated_types.size(); ++i){
            addRelation(from.aggregated_types[i], to.aggregated_types[i]);
        }
    }
    else throw std::runtime_error("adding relation between" + from.to_string() + " and " + to.to_string());
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
        for(unsigned int i = 0; i < type.aggregated_types.size(); ++i){
            type.aggregated_types[i] = doRenumerations(type.aggregated_types[i], renumerations);
        }
        return type;
    }
}

Type Environment::renumeratedToUnique(Type type)
{
    type = followRelations(type);
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

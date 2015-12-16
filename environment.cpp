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
        iter->second = newType.getMoreSpecific(iter->second);
    }
    else throw std::runtime_error("identifier not found");
}

Type Environment::getIdentifierType(Identifier identifier)
{
    for(auto iter = identifier_types.rbegin(); iter != identifier_types.rend(); ++iter){
        if(iter->first.name == identifier.name) return iter->second;
    }
    for(auto iter = env.rbegin(); iter != env.rend(); ++iter){
        if(iter->first.name == identifier.name) return iter->second->exp_type;
    }
    throw std::runtime_error("identifier not found");
}

Type Environment::get_new_polymorphic_type()
{
    return Type(POLYMORPHIC, "'" + std::to_string(polymorphic_types_in_statement++));
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

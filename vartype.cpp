#include "vartype.h"
#include <exception>


Type Type::getMoreGeneral(Type other_type)
{
    if(this->relatedWith(other_type)){
        // in all of these ifs' belowe, we are assuming that these types are RELATED
        if(this->type_enum == UNDETERMINED) return *this;
        else if(other_type.type_enum == UNDETERMINED) return other_type;
        else if(this->type_enum == POLYMORPHIC) return *this;
        else if(other_type.type_enum == POLYMORPHIC) return other_type;
        else if(this->type_enum == COMPLEX && other_type.type_enum == COMPLEX){
            Type moreGeneral(COMPLEX, this->type_name, this->constructor_name);
            for(unsigned int i=0; i<this->aggregated_types.size(); ++i){
                moreGeneral.aggregated_types.push_back( aggregated_types[i].getMoreGeneral(other_type.aggregated_types[i]) );
            }
            return moreGeneral;
        }
        else if(this->type_enum == FUNCTION_TYPE && other_type.type_enum == FUNCTION_TYPE){
            return Type(FUNCTION_TYPE, type_name, constructor_name, std::vector<Type>{ this->aggregated_types[0].getMoreGeneral(other_type.aggregated_types[0]),
                                                                                       this->aggregated_types[1].getMoreGeneral(other_type.aggregated_types[1]) });
        }
        else if(this->type_enum == PRIMITIVE && other_type.type_enum == PRIMITIVE) return *this;
        else throw std::runtime_error("Algorithm error! getMoreGeneral()");
    }
    else{
        throw std::runtime_error("Trying to get more general type from 2 unrelated types.");
    }
}

Type Type::getMoreSpecific(Type other_type)
{
    if(this->relatedWith(other_type)){
        // in all of these ifs' belowe, we are assuming that these types are RELATED
        if(this->type_enum == UNDETERMINED) return other_type;
        else if(other_type.type_enum == UNDETERMINED) return *this;
        else if(this->type_enum == POLYMORPHIC) return other_type;
        else if(other_type.type_enum == POLYMORPHIC) return *this;
        else if(this->type_enum == COMPLEX && other_type.type_enum == COMPLEX){
            Type moreSpecific(COMPLEX, this->type_name, this->constructor_name);
            for(unsigned int i=0; i<this->aggregated_types.size(); ++i){
                moreSpecific.aggregated_types.push_back( aggregated_types[i].getMoreSpecific(other_type.aggregated_types[i]) );
            }
            return moreSpecific;
        }
        else if(this->type_enum == FUNCTION_TYPE && other_type.type_enum == FUNCTION_TYPE){
            return Type(FUNCTION_TYPE, type_name, constructor_name, std::vector<Type>{ this->aggregated_types[0].getMoreSpecific(other_type.aggregated_types[0]),
                                                                                       this->aggregated_types[1].getMoreSpecific(other_type.aggregated_types[1]) });
        }
        else if(this->type_enum == PRIMITIVE && other_type.type_enum == PRIMITIVE) return *this;
        else throw std::runtime_error("Algorithm error! getMoreSpecific()");
    }
    else{
        throw std::runtime_error("Trying to get more specific type from 2 unrelated types.");
    }
}

bool Type::relatedWith(Type other_type)
{
    if(this->type_enum == UNDETERMINED || other_type.type_enum == UNDETERMINED) return true;
    else if(this->type_enum == POLYMORPHIC || other_type.type_enum == POLYMORPHIC) return true;
    else if(this->type_enum == COMPLEX && other_type.type_enum == COMPLEX && this->aggregated_types.size() == other_type.aggregated_types.size()){
        bool related = (this->type_name == other_type.type_name && this->constructor_name == other_type.constructor_name);
        for(unsigned int i=0; i<this->aggregated_types.size() && related; ++i){
            related = related || aggregated_types[i].relatedWith(other_type.aggregated_types[i]);
        }
        return related;
    }
    else if(this->type_enum == FUNCTION_TYPE && other_type.type_enum == FUNCTION_TYPE){
        return this->aggregated_types[0].relatedWith(other_type.aggregated_types[0]) && this->aggregated_types[1].relatedWith(other_type.aggregated_types[1]);
    }
    else if(this->type_enum == PRIMITIVE && other_type.type_enum == PRIMITIVE && this->type_name == other_type.type_name) return true;
    else return false;
}

Type Type::withTypeSwapped(Type fromType, Type toType)
{
    if(*this == fromType) return toType;
    else{
        Type copy = *this;
        for(unsigned int i = 0; i < aggregated_types.size(); ++i){
            copy.aggregated_types[i] = aggregated_types[i].withTypeSwapped(fromType, toType);
        }
        return copy;
    }
}

// 'this' is a pattern
// to is an 'argument'
void Type::getTypeMapping(std::map<Type, Type>& mapping, Type to)
{
    if(relatedWith(to)){
        if(type_enum == POLYMORPHIC){
            mapping[*this] = mapping[*this].getMoreSpecific(to);
        }
        else{
            for(unsigned int i = 0; i<aggregated_types.size(); ++i){
                aggregated_types[i].getTypeMapping(mapping, to.aggregated_types[i]);
            }
        }
    }
    else throw std::runtime_error("trying to obtain mapping between unrelated types");
}

Type Type::withAppliedMapping(const std::map<Type, Type> &mapping)
{
    if(type_enum == POLYMORPHIC){
        if(mapping.find(*this) != mapping.end()) return mapping.find(*this)->second;
        else return *this;
    }
    else{
        Type result = *this;
        for(unsigned int i = 0; i<aggregated_types.size(); ++i){
            result.aggregated_types[i] = aggregated_types[i].withAppliedMapping(mapping);
        }
        return result;
    }
}

Type Type::withArgumentApplied(Type argumentType)
{
    if(type_enum == FUNCTION_TYPE){
        std::map<Type, Type> type_mapping;
        aggregated_types[0].getTypeMapping(type_mapping, argumentType);
        return aggregated_types[1].withAppliedMapping(type_mapping);
    }
    else throw std::runtime_error("deducing return type of non-function");
}

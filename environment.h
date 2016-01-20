#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <map>
#include <deque>
#include <list>
#include <algorithm>
#include <vector>
#include "value.h"
#include "vartype.h"

class Identifier;
class Value;
class TypeDefAST;

class Environment
{
public:
    Environment():polymorphic_types_in_statement(0), execution_inside_pattern(false){ displayed_values = 0; }

    void addActivationFrame();
    void removeActivationFrame();

    void addIdentifierToBeTypeDeduced(Identifier identifier, bool rec = false, Type startingType = Type());
    void setIdentifierType(Identifier identifier, Type newType);
    void resetIdentifierType(Identifier identifier, Type newType);
    Type getIdentifierType(Identifier identifier);


    Type getNewPolymorphicType();
    void reset_polymorphic_types();
    int polymorphic_types_in_statement;

    void addValue(Identifier identifier, Value* value);
    Value* getValue(Identifier identifier);
    bool valueExists(Identifier identifier);

    int displayed_values;
    void printNewValues();

    std::list< std::pair<Identifier, Type> > identifier_types;

    // pointers used so values stored could be polymorphic
    std::list< Identifier > identifiers_stack;
    std::map< Identifier, std::list<Value*> > variables;
    std::map< Identifier, Type > type_constructors;

    void addTypes(std::vector<TypeDefAST*> type_defs);
    void addType(TypeDefAST* type_def);
    Type getType(Identifier identifier);

    void cleanupAfterStatement();

/** type relations part **/
    Type followRelations(Type type, int depth = 0);
    void addFunctionCallRelations(Type function_type, Type argument_applied, Type result_expected);
    void addRelation(Type from, Type to);
    void checkForCircularReferences();
    void clearRelations();

    Type renumeratedToSmallest(Type type);
    Type renumeratedToUnique(Type type);

    std::string relationsToString();

    bool execution_inside_pattern;

private:
    Type doRenumerations(Type type, std::map<Type, Type>& renumerations);

    std::map<Type, Type> type_relations;
};

#include "ast.h"

#endif // ENVIRONMENT_H

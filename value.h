#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <list>
#include <vector>
#include <functional>

using namespace std;

class Value;

typedef function<Value(Value)> fun;

enum ValueType{
    INT_VALUE,
    FLOAT_VALUE,
    STRING_VALUE,
    BOOL_VALUE,
    LIST_VALUE,
    TUPLE_VALUE,
    //RECORD_VALUE
    FUNCTION_VALUE
};

union ValueContainer{
    int int_value;
    double float_value;
    string *string_value;
    bool bool_value;
    list<Value> *list_value;
    vector<Value> *tuple_value;
    //map<string, Value> record_value
    fun *function_value;
};

class Value
{
public:
    Value(ValueType type = INT_VALUE);

    ValueType type;
    ValueContainer container;
};

#endif // VALUE_H

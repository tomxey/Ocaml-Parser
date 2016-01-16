/* main.cc */

#include <set>
#include "heading.h"
#include "value.h"
#include "ast.h"
#include <unistd.h>
#include "parseessentials.h"

// prototype of bison-generated parser function
int yyparse();

int factorial;

/**
TODO:
    - look at polymorphic types' names 'a 'b
        make args of type 'a 'b instead of UNDETERMINED
        when getting 'moreSpecificType' pay attention to polymorphic types of the same id
        eg. ('a * 'a) ISN'T related with ( int * float )

    - local type mapping, if function: ('a -> 'b) -> 'b is supplied with ('c -> 'd) argument then it doesn't mean that everywhere 'c = 'a and 'd = 'b, or does it?
 */

int main(int argc, char **argv)
{
    ParseEssentials::toplevel_environment.addType(new TypeDefAST("int"));;
    ParseEssentials::toplevel_environment.addType(new TypeDefAST("bool"));;
    ParseEssentials::toplevel_environment.addType(new TypeDefAST("float"));;
    ParseEssentials::toplevel_environment.addType(new TypeDefAST("string"));;
    ParseEssentials::toplevel_environment.addType(new TypeDefAST("list", std::vector<std::string>{"'a"}, std::vector<std::pair<std::string, Type> >{
                                                                     std::pair<std::string, Type>("End", Type()),
                                                                     std::pair<std::string, Type>("Elem", Type(COMPLEX, "2tuple", std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(COMPLEX, "list", std::vector<Type>{Type(POLYMORPHIC, "'a")})})) } ));;
    ParseEssentials::toplevel_environment.addType(new TypeDefAST("unit", std::vector<std::string>(), std::vector<std::pair<std::string, Type> >{ std::pair<std::string, Type>("Unit", Type()) }));
    ParseEssentials::toplevel_environment.addType(new TypeDefAST("<fun>", std::vector<std::string>{"'a", "'b"}));

    //ParseEssentials::toplevel_environment.addValue(Identifier("print_int"), new BuiltIn_Function([](Value* arg)->Value* {std::cout << ((Integer*)arg)->value << std::endl; return arg;}, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int") ));
    DEFINE_CURRIED_FUNCTION("+",a, b, return new Integer(((Integer*)a)->value + ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("-",a, b, return new Integer(((Integer*)a)->value - ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("*",a, b, return new Integer(((Integer*)a)->value * ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("/",a, b, if(((Integer*)b)->value == 0) throw std::runtime_error("division by zero at: " + to_string(((Integer*)a)->value) + " / 0"); return new Integer(((Integer*)a)->value / ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("%",a, b, if(((Integer*)b)->value == 0) throw std::runtime_error("division by zero at: " + to_string(((Integer*)a)->value) + " % 0"); return new Integer(((Integer*)a)->value % ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("=",a, b, return new Bool(a->equals(b));, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION("<>",a, b, return new Bool(!a->equals(b));, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION("==",a, b, return new Bool(a==b);, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION("!=",a, b, return new Bool(a!=b);, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION("<",a, b, return new Bool(a->smallerThan(b));, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION("<=",a, b, return new Bool(!b->smallerThan(a));, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION(">",a, b, return new Bool(b->smallerThan(a));, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION(">=",a, b, return new Bool(!a->smallerThan(b));, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'a"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION("+.",a, b, return new Float(((Float*)a)->value + ((Float*)b)->value);, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"));
    DEFINE_CURRIED_FUNCTION("-.",a, b, return new Float(((Float*)a)->value - ((Float*)b)->value);, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"));
    DEFINE_CURRIED_FUNCTION("*.",a, b, return new Float(((Float*)a)->value * ((Float*)b)->value);, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"));
    DEFINE_CURRIED_FUNCTION("/.",a, b, return new Float(((Float*)a)->value / ((Float*)b)->value);, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"));
    DEFINE_CURRIED_FUNCTION("**",a, b, return new Float( std::pow(((Float*)a)->value , ((Float*)b)->value));, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float"));
    DEFINE_CURRIED_FUNCTION("^",a, b, return new String(((String*)a)->value + ((String*)b)->value);, Type(PRIMITIVE, "string"), Type(PRIMITIVE, "string"), Type(PRIMITIVE, "string"));
    DEFINE_CURRIED_FUNCTION("||",a, b, return new Bool(((Bool*)a)->value || ((Bool*)b)->value);, Type(PRIMITIVE, "bool"), Type(PRIMITIVE, "bool"), Type(PRIMITIVE, "bool"));
    DEFINE_CURRIED_FUNCTION("&&",a, b, return new Bool(((Bool*)a)->value && ((Bool*)b)->value);, Type(PRIMITIVE, "bool"), Type(PRIMITIVE, "bool"), Type(PRIMITIVE, "bool"));

    ParseEssentials::toplevel_environment.addValue(Identifier("float_of_int"), new BuiltIn_Function([](Value* arg)->Value* {return new Float(((Integer*)arg)->value);}, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "float") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("int_of_float"), new BuiltIn_Function([](Value* arg)->Value* {return new Integer(((Float*)arg)->value);}, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "int") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("floor"), new BuiltIn_Function([](Value* arg)->Value* {return new Float(std::floor(((Float*)arg)->value));}, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("ceil"), new BuiltIn_Function([](Value* arg)->Value* {return new Float(std::ceil(((Float*)arg)->value));}, Type(PRIMITIVE, "float"), Type(PRIMITIVE, "float") ));

    ParseEssentials::toplevel_environment.addValue(Identifier("hd"), new BuiltIn_Function([](Value* arg)->Value* {if(((ComplexValue*)arg)->aggregatedValues.size() == 0) throw std::runtime_error("hd(End)"); return ((ComplexValue*)(((ComplexValue*)arg)->aggregatedValues[0]))->aggregatedValues[0];}, Type(COMPLEX, "list",std::vector<Type>{Type(POLYMORPHIC, "'a")}), Type(POLYMORPHIC, "'a") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("tl"), new BuiltIn_Function([](Value* arg)->Value* {if(((ComplexValue*)arg)->aggregatedValues.size() == 0) throw std::runtime_error("tl(End)"); return ((ComplexValue*)(((ComplexValue*)arg)->aggregatedValues[0]))->aggregatedValues[1];}, Type(COMPLEX, "list",std::vector<Type>{Type(POLYMORPHIC, "'a")}), Type(COMPLEX, "list",std::vector<Type>{Type(POLYMORPHIC, "'a")}) ));

    ParseEssentials::toplevel_environment.addValue(Identifier("fst"), new BuiltIn_Function([](Value* arg)->Value* {return ((ComplexValue*)arg)->aggregatedValues[0];}, Type(COMPLEX, "2tuple",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}), Type(POLYMORPHIC, "'a") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("snd"), new BuiltIn_Function([](Value* arg)->Value* {return ((ComplexValue*)arg)->aggregatedValues[1];}, Type(COMPLEX, "2tuple",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}), Type(POLYMORPHIC, "'b") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("exit"), new BuiltIn_Function([](Value* arg)->Value* { exit( ((Integer*)arg)->value );}, Type(PRIMITIVE, "int"), Type(POLYMORPHIC, "'a") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("not"), new BuiltIn_Function([](Value* arg)->Value* {return new Bool(!(((Bool*)arg)->value));}, Type(PRIMITIVE, "bool"), Type(PRIMITIVE, "bool") ));

    ParseEssentials::toplevel_environment.addValue(Identifier("failwith"), new BuiltIn_Function([](Value* arg)->Value* {throw std::runtime_error(((String*)arg)->value);}, Type(PRIMITIVE, "string"), Type(POLYMORPHIC, "'a") ));

  if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
  {
    cerr << argv[0] << ": File " << argv[1] << " cannot be opened.\n";
    exit( 1 );
  }

  while(!feof(stdin)){
    yyparse();
  }

  return 0;
}


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
    ParseEssentials::toplevel_environment.addValue(Identifier("print_int"), new BuiltIn_Function([](Value* arg)->Value* {std::cout << ((Integer*)arg)->value << std::endl; return arg;}, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int") ));
    DEFINE_CURRIED_FUNCTION("+",a, b, return new Integer(((Integer*)a)->value + ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("-",a, b, return new Integer(((Integer*)a)->value - ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("*",a, b, return new Integer(((Integer*)a)->value * ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("/",a, b, if(((Integer*)b)->value == 0) throw std::runtime_error("division by zero at: " + to_string(((Integer*)a)->value) + " / 0"); return new Integer(((Integer*)a)->value / ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
    DEFINE_CURRIED_FUNCTION("=",a, b, return new Bool(((Integer*)a)->value == ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "bool"));

    DEFINE_CURRIED_FUNCTION("Pair",a, b, return new ComplexValue(Type(COMPLEX, "pair",std::vector<Type>{a->exp_type, b->exp_type}), "Pair",std::vector<Value*>{a, b});, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b"), Type(COMPLEX, "pair",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}));
    ParseEssentials::toplevel_environment.addValue(Identifier("fst"), new BuiltIn_Function([](Value* arg)->Value* {return ((ComplexValue*)arg)->aggregatedValues[0];}, Type(COMPLEX, "pair",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}), Type(POLYMORPHIC, "'a") ));
    ParseEssentials::toplevel_environment.addValue(Identifier("snd"), new BuiltIn_Function([](Value* arg)->Value* {return ((ComplexValue*)arg)->aggregatedValues[1];}, Type(COMPLEX, "pair",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}), Type(POLYMORPHIC, "'b") ));

    ParseEssentials::toplevel_environment.addValue(Identifier("exit"), new BuiltIn_Function([](Value* arg)->Value* { exit( ((Integer*)arg)->value );}, Type(PRIMITIVE, "int"), Type(POLYMORPHIC, "'a") ));

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


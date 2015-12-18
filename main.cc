/* main.cc */

#include <set>
#include "heading.h"
#include "value.h"
#include "ast.h"
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

  if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
  {
    cerr << argv[0] << ": File " << argv[1] << " cannot be opened.\n";
    exit( 1 );
  }
  
  yyparse();

  ParseEssentials::toplevel_environment.addValue(Identifier("print_int"), new BuiltIn_Function([](Value* arg)->Value* {std::cout << ((Integer*)arg)->value << std::endl; return arg;}, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int") ));
  DEFINE_CURRIED_FUNCTION("+",a, b, return new Integer(((Integer*)a)->value + ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
  DEFINE_CURRIED_FUNCTION("-",a, b, return new Integer(((Integer*)a)->value - ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
  DEFINE_CURRIED_FUNCTION("*",a, b, return new Integer(((Integer*)a)->value * ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
  DEFINE_CURRIED_FUNCTION("/",a, b, return new Integer(((Integer*)a)->value / ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"));
  DEFINE_CURRIED_FUNCTION("=",a, b, return new Bool(((Integer*)a)->value == ((Integer*)b)->value);, Type(PRIMITIVE, "int"), Type(PRIMITIVE, "int"), Type(PRIMITIVE, "bool"));

  DEFINE_CURRIED_FUNCTION("tuple",a, b, return new ComplexValue(Type(COMPLEX, "tuple", "",std::vector<Type>{a->exp_type, b->exp_type}), std::vector<Value*>{a, b});, Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b"), Type(COMPLEX, "tuple", "",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}));
  ParseEssentials::toplevel_environment.addValue(Identifier("fst"), new BuiltIn_Function([](Value* arg)->Value* {return ((ComplexValue*)arg)->aggregatedValues[0];}, Type(COMPLEX, "tuple", "",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}), Type(POLYMORPHIC, "'a") ));
  ParseEssentials::toplevel_environment.addValue(Identifier("snd"), new BuiltIn_Function([](Value* arg)->Value* {return ((ComplexValue*)arg)->aggregatedValues[1];}, Type(COMPLEX, "tuple", "",std::vector<Type>{Type(POLYMORPHIC, "'a"), Type(POLYMORPHIC, "'b")}), Type(POLYMORPHIC, "'b") ));

  std::set<int> properStatements;

  std::cout << "Number of statements: " << ParseEssentials::toplevel_statements.size() << std::endl;

  int i = 0;
  for(Statement* statement : ParseEssentials::toplevel_statements){
      std::cout << "-->Statement " << i++ << std::endl;
      std::cout << statement->print(0) << std::endl;
  }

  std::cout << "---------Determine Types---------------" << std::endl;
  i = 0;
  for(Statement* statement : ParseEssentials::toplevel_statements){
      std::cout << "-->Statement " << i << std::endl;
      int deductionAttempts = 0;
      try{
          deductionAttempts++;
          Type type, newType;
          type = newType = statement->deduceType(ParseEssentials::toplevel_environment, Type());
          do{
              type = newType;
              deductionAttempts++;
              newType = statement->deduceType(ParseEssentials::toplevel_environment, type);
          } while(type != newType);
        std::cout << newType << std::endl;
        properStatements.insert(i);
      } catch(std::runtime_error ex){
          std::cout << "Type deduction for statement " << i << " failed:\n" << ex.what() << std::endl;
      }
      std::cout << "Deduced " << deductionAttempts << " times..." << std::endl;
      ++i;
  }

  std::cout << "---------    Execute    ---------------" << std::endl;
  i = 0;
  for(Statement* statement : ParseEssentials::toplevel_statements){
      std::cout << "-->Statement " << i << std::endl;
      if(properStatements.find(i) != properStatements.end()){
          Value* retVal = statement->execute(ParseEssentials::toplevel_environment);
          if(retVal != nullptr){
              std::cout << retVal->print(1) << std::endl;
          }
          else{
              std::cout << "Some statement, no value to print..." << std::endl;
          }
      }
      else{
          std::cout << "Statement " << i << " not proper. Skipping..." << std::endl;
      }
      ++i;
  }

  return 0;
}


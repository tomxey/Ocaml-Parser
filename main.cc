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
      try{
        std::cout << statement->deduceType(ParseEssentials::toplevel_environment, Type()) << std::endl;
        properStatements.insert(i);
      } catch(std::runtime_error ex){
          std::cout << "Type deduction for statement " << i << " failed:\n" << ex.what() << std::endl;
      }
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


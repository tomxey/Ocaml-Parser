/* main.cc */

#include "heading.h"
#include "value.h"
#include "ast.h"
#include "parseessentials.h"

// prototype of bison-generated parser function
int yyparse();

  int factorial;

int main(int argc, char **argv)
{
    cout << ParseEssentials::get_CPP_headers() << endl;
    cout << ParseEssentials::get_CPP_main_start() << endl;
    cout << ParseEssentials::get_and_register_CPP_curried_function_for_infix_operator(FUN_NAME_PLUS, "+", "int", "int", "int") << endl;
    cout << ParseEssentials::get_and_register_CPP_curried_function_for_infix_operator(FUN_NAME_MULT, "*", "int", "int", "int") << endl;
    cout << ParseEssentials::get_and_register_CPP_curried_function_for_infix_operator(FUN_NAME_MINUS, "-", "int", "int", "int") << endl;
    cout << ParseEssentials::get_and_register_CPP_curried_function_for_infix_operator(FUN_NAME_DIV, "/", "int", "int", "int") << endl;
    cout << ParseEssentials::get_and_register_CPP_curried_function_for_infix_operator(FUN_NAME_EQUALS, "==", "int", "int", "bool") << endl;
    cout << ParseEssentials::get_and_register_CPP_function(FUN_NAME_PRINT_INT, "num", "int", "int", "std::cout << num << std::endl; return num;") << endl;

  if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
  {
    cerr << argv[0] << ": File " << argv[1] << " cannot be opened.\n";
    exit( 1 );
  }
  
  yyparse();

  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  ParseEssentials::treeRoot->determineType();
  cout << ParseEssentials::treeRoot->parseTree() << endl;

  cout << ParseEssentials::get_CPP_main_end() << endl;

  //auto test = [](auto a){ return a*a; };

    {
      function< int(int) > factorial = [&factorial](int n)->int{ if(n==0) return 1; else return n*factorial(n-1); };

      int result = factorial(10);

      {
          extern int factorial;
          factorial = 10;
      }
    }
  return 0;
}


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

  if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
  {
    cerr << argv[0] << ": File " << argv[1] << " cannot be opened.\n";
    exit( 1 );
  }
  
  yyparse();

  std::cout << "Number of statements: " << ParseEssentials::toplevel_statements.size() << std::endl;

  for(Statement* statement : ParseEssentials::toplevel_statements){
      std::cout << "Next statement:" << std::endl;
      std::cout << statement->print(0) << std::endl;
  }

  return 0;
}


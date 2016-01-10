/* occ */
/* occ.y */

%{
#include "heading.h"
int yyerror(char *s);
//int yylex(void);
extern "C" int yylex();
extern "C" FILE *yyin;
%}

%union{
  int		int_val;
  double	float_val;
  string*	string_val;
  bool          bool_val;
  AST*          ast_elem;
  Statement*    statement;
  Expression*   expresion;
  Identifier*   identifier;
  vector<Statement*>* statements;
}

%start	input 

%token	<int_val>	INTEGER_LITERAL
%token	<string_val>	IDENTIFIER
%token	<string_val>	STRING_LITERAL
%token  <float_val>     FLOAT_LITERAL
%token	<string_val>	VALUE_CONSTRUCTOR
%token	<string_val>	POLYMORPHIC_TYPE

%type	<statement>	statement
%type	<statements>	statements
%type	<expresion>	exp
%type	<bool_val>	RECS

// %left associative a + b + c = (a + b) + c
// %right associative ...
// %nonassoc    a + b + c = forbidden
// the further the op is declared here, the higher precedence it has

%nonassoc   TYPE OF

%right	FUNCTION
%right	INTO

%left '('
%left ')'

%left IDENTIFIER
%left INTEGER_LITERAL

%left	LET
%left	REC
%left   IN

%right	IF
%right	THEN ELSE

%left	EQUALS

%left	PLUS MINUS
%left	MULT DIV

%left FUNAPPLY
%left SEMIC2

%%

input:  /* empty */
        | statements      { ParseEssentials::toplevel_statements.insert(ParseEssentials::toplevel_statements.end(), $1->begin(), $1->end()); }
        ;

statements:     statement               { $$ = new vector<Statement*>{$1}; }
        |       statements statement    { $1->push_back($2);  $$ = $1; }
        ;

statement:      LET RECS IDENTIFIER EQUALS exp SEMIC2 { $$ = new Let(new Identifier(*$3), $5, $2); ParseEssentials::parseStatement($$); }
        |       exp SEMIC2 { $$ = $1; ParseEssentials::parseStatement($$); }
        ;

exp:        INTEGER_LITERAL	{ $$ = new Integer($1); }
        |   STRING_LITERAL	{ $$ = new String( *($1) ); }
        |   FLOAT_LITERAL	{ $$ = new Float($1); }
        |   exp PLUS exp	{ $$ = new FunctionCall( new FunctionCall(new Identifier("+"), $1) , $3); }
        |   exp MINUS exp	{ $$ = new FunctionCall( new FunctionCall(new Identifier("-"), $1) , $3); }
        |   exp DIV exp         { $$ = new FunctionCall( new FunctionCall(new Identifier("/"), $1) , $3); }
        |   exp MULT exp	{ $$ = new FunctionCall( new FunctionCall(new Identifier("*"), $1) , $3); }
        |   exp EQUALS exp      { $$ = new FunctionCall( new FunctionCall(new Identifier("="), $1) , $3); }
        |   IDENTIFIER          { $$ = new Identifier( *$1 ); }
        |   exp exp   %prec FUNAPPLY    { $$ = new FunctionCall($1, $2); }
        |   LET RECS IDENTIFIER EQUALS exp IN exp   { $$ = new LetIn(new Identifier(*$3), $5, $2, $7); }
        |   IF exp THEN exp ELSE exp    { $$ = new Conditional($2, $4, $6); }
        |   FUNCTION IDENTIFIER INTO exp    { $$ = new Function(new Identifier(*$2), $4);}
        |   '(' exp ')'   { $$ = $2; }
        ;

RECS:   /* empty */ { $$ = false; }
        | REC       { $$ = true; }
        ;

%%

int yyerror(string s)
{
  extern int yylineno;	// defined and maintained in lex.c
  extern char *yytext;	// defined and maintained in lex.c
  
  cerr << "ERROR: " << s << " at symbol \"" << yytext;
  cerr << "\" on line " << yylineno << endl;
  //exit(1);
  return 0;
}

int yyerror(char *s)
{
  return yyerror(string(s));
}



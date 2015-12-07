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
  string*	op_val;
  bool      bool_val;
  AST*      tree_elem;
}

%start	input 

%token	<int_val>	INTEGER_LITERAL
%token	<op_val>	IDENTIFIER
%token	<op_val>	STRING_LITERAL
%token  <float_val>     FLOAT_LITERAL
%type	<tree_elem>	exp
%type	<tree_elem>	statement
%type	<tree_elem>	statements
%type	<bool_val>	RECS

// %left associative a + b + c = (a + b) + c
// %right associative ...
// %nonassoc    a + b + c = forbidden
// the further the op is declared here, the higher precedence it has

//%left COMMENTED_LINE

%right	FUNCTION
%right	INTO

%left IDENTIFIER
%left INTEGER_LITERAL

%left	LET
%left	REC

%left	EQUALS

%right	IF
%right	THEN ELSE

%left	PLUS MINUS
%left	MULT DIV

//%left <tree_elem> exp
%left FUNAPPLY

%left SEMIC2

%%

input:		/* empty */
        //statements {ParseEssentials::treeRoot = $1 ;}
        //statement { cout << ($1)->parseTree() << endl; delete $1; }
        statements { ParseEssentials::treeRoot = $1 ; cout << "/*\n" << ($1)->printTree() << "\n*/" << endl; }
		;

statement:   LET RECS IDENTIFIER EQUALS exp SEMIC2 { $$ = new AST(VariableDeclaration, new AST(Identifier, *($3)), $5, $2); }
         |   exp SEMIC2 { $$ = new AST($1, NULL) ; }
         ;

statements:     statement               { $$ = $1; }
          |     statement statements    { $$ = new AST( $1, $2 ); }

exp:		INTEGER_LITERAL	{ $$ = new AST($1); }
        |   STRING_LITERAL	{ $$ = new AST(StringLiteral, *($1) ); }
        |   FLOAT_LITERAL	{ $$ = new AST($1); }
        | exp PLUS exp	{ $$ = new AST(FunctionCall, new AST(FunctionCall, new AST(Identifier, FUN_NAME_PLUS), $1), $3); }
        | exp MINUS exp	{ $$ = new AST(FunctionCall, new AST(FunctionCall, new AST(Identifier, FUN_NAME_MINUS), $1), $3); }
        | exp DIV exp	{ $$ = new AST(FunctionCall, new AST(FunctionCall, new AST(Identifier, FUN_NAME_DIV), $1), $3); }
        | exp MULT exp	{ $$ = new AST(FunctionCall, new AST(FunctionCall, new AST(Identifier, FUN_NAME_MULT), $1), $3); }
        | exp EQUALS exp    { $$ = new AST(FunctionCall, new AST(FunctionCall, new AST(Identifier, FUN_NAME_EQUALS), $1), $3); }
        | IDENTIFIER            { $$ = new AST(Identifier, *($1)); }
        | exp exp   %prec FUNAPPLY { $$ = new AST(FunctionCall, new AST(*($1)), $2); }
        | IF exp THEN exp ELSE exp { $$ = new AST($2, $4, $6); }
        | FUNCTION exp INTO exp { $$ = new AST(FunctionLiteral, new AST(*($2)), $4); }
        | '(' exp ')'		{ $$ = $2; }
		;

RECS:   /* empty */ { $$ = false; }
        | REC     { $$ = true; }

//SEMIC2_PLUS_COMMENTED:  SEMIC2
//                     |  SEMIC2 COMMENTED_LINE statement
//                    ;

%%

int yyerror(string s)
{
  extern int yylineno;	// defined and maintained in lex.c
  extern char *yytext;	// defined and maintained in lex.c
  
  cerr << "ERROR: " << s << " at symbol \"" << yytext;
  cerr << "\" on line " << yylineno << endl;
  exit(1);
}

int yyerror(char *s)
{
  return yyerror(string(s));
}



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
  Expression*   expression;
  Identifier*   identifier;
  Type*         type;
  vector<Type>* types;
  vector<string>*     strings;
  vector<Statement*>* statements;
  pair<string, Type>*       value_constructor_definition;
  vector< pair<string, Type> >*  value_constructors_definitions;
  Let*      let_statement;
  vector<Expression*>* comma_separated_expressions;
  vector< pair<Expression*, Expression*> >*     patterns_and_cases;
  TypeDefAST*   type_def;
  std::vector<TypeDefAST*>* type_defs;
}

%start	input 

%token	<int_val>	INTEGER_LITERAL
%token	<string_val>	IDENTIFIER
%token	<string_val>	STRING_LITERAL
%token  <float_val>     FLOAT_LITERAL
%token  <bool_val>      BOOLEAN_LITERAL
%token	<string_val>	VALUE_CONSTRUCTOR
%token	<string_val>	POLYMORPHIC_TYPE
%token	<string_val>	INFIX_OP
%token	<string_val>	PREFIX_OP


%type	<statement>	statement
%type	<statements>	statements
%type	<expression>	exp
%type	<expression>	pattern
%type	<bool_val>	RECS
%type   <strings>       polymorphic_types_list
%type   <strings>       comma_separated_polymorphic_types

%type   <types>         comma_separated_types
//%type   <types>         type_parameters
%type   <type>          type

%type   <value_constructors_definitions>    value_constructors_definitions
%type   <value_constructor_definition>      value_constructor_definition

%type   <let_statement> let_statement

%type   <comma_separated_expressions>   comma_separated_expressions
%type   <comma_separated_expressions>   semicolon_separated_expressions
%type   <expression>    tuple
%type   <type>          tuple_type
%type   <types>         asterisk_separated_types

%type   <patterns_and_cases>    patterns_and_cases

%type   <type_def>  type_def
%type   <type_defs> type_defs
// %left associative a + b + c = (a + b) + c
// %right associative ...
// %nonassoc    a + b + c = forbidden
// the further the op is declared here, the higher precedence it has

%nonassoc   TYPE OF
%left   AND

%right	FUNCTION
%right	INTO

%right  MATCH WITH

%right	LET REC IN

%right	IF
%right	THEN ELSE

%left	'='

%left   INFIX_OP
%left	'+' '-'
%left	'*' '/'
%right LIST_CONS

%left IDENTIFIER INTEGER_LITERAL STRING_LITERAL FLOAT_LITERAL VALUE_CONSTRUCTOR POLYMORPHIC_TYPE BOOLEAN_LITERAL

%right PREFIX_OP
%left '(' ')' '[' ']'
%right  ','
%left FUNAPPLY
%right   '|'
%left SEMIC2

%%

input:  /* empty */
        | statements      { ParseEssentials::toplevel_statements.insert(ParseEssentials::toplevel_statements.end(), $1->begin(), $1->end()); }
        ;

statements:     statement               { $$ = new vector<Statement*>{$1}; }
        |       statements statement    { $1->push_back($2);  $$ = $1; }
        ;

statement:      exp SEMIC2 { $$ = $1; ParseEssentials::parseStatement($$); }
        |       let_statement SEMIC2 { $$ = $1; ParseEssentials::parseStatement($$); }
        |       TYPE type_defs SEMIC2   {$$ = new TypeDefsAST(*$2); ParseEssentials::parseStatement($$); }
        ;

type_defs:  type_def    { $$ = new std::vector<TypeDefAST*>{$1}; }
         |  type_defs AND type_def  { $$ = $1; $$->push_back($3); }
         ;

type_def:   polymorphic_types_list IDENTIFIER '=' value_constructors_definitions { $$ = new TypeDefAST(*$2, *$1, *$4); delete $1; delete $2; delete $4;}
        ;

polymorphic_types_list:     /* empty */     { $$ = new vector<string>(); }
                      |    POLYMORPHIC_TYPE     { $$ = new vector<string>{*$1}; delete $1; }
                      |    '(' comma_separated_polymorphic_types ')'    { $$ = $2; }
                      ;

comma_separated_polymorphic_types:      POLYMORPHIC_TYPE    { $$ = new vector<string>{*$1}; delete $1; }
                                 |      comma_separated_polymorphic_types ',' POLYMORPHIC_TYPE { $$ = $1; $$->push_back(*$3); delete $3; }
                                 ;


value_constructors_definitions:     value_constructor_definition    { $$ = new vector<pair<string,Type> >{*$1}; delete $1; }
                              |     value_constructors_definitions '|' value_constructor_definition { $$ = $1; $$->push_back(*$3); delete $3; }
                              ;

value_constructor_definition:       VALUE_CONSTRUCTOR   { $$ = new pair<string, Type>(*$1, Type()); delete $1; }
                            |       VALUE_CONSTRUCTOR OF type   { $$ = new pair<string, Type>(*$1, *$3); delete $1; delete $3; }
                            ;

type:       type IDENTIFIER  { $$ = new Type(COMPLEX, *$2, vector<Type>{*$1}); delete $1; delete $2; }
    |       '(' comma_separated_types ')' IDENTIFIER  { $$ = new Type(COMPLEX, *$4, *$2); delete $2; delete $4; }
    |       IDENTIFIER                  { $$ = new Type(COMPLEX, *$1); delete $1; }
    |       POLYMORPHIC_TYPE            { $$ = new Type(POLYMORPHIC, *$1); delete $1; }
    |       tuple_type                  { $$ = $1; }
    |       '(' type ')'                { $$ = $2; }
    |       '(' type INTO type ')'      { $$ = new Type(FUNCTION_TYPE, "", std::vector<Type>{*$2, *$4}); delete $2; delete $4; }
    ;

comma_separated_types:      type ',' type        { $$ = new vector<Type>{*$1, *$3}; delete $1; delete $3; }
                     |      comma_separated_types ',' type { $$ = $1; $1->push_back(*$3); delete $3; }
                     ;

tuple_type:     asterisk_separated_types    { $$ = new Type(COMPLEX, to_string($1->size()) + "tuple", *$1); delete $1; }
          ;

asterisk_separated_types:   type '*' type       { $$ = new vector<Type>{*$1, *$3}; delete $1; delete $3; }
                        |   asterisk_separated_types '*' type   { $$ = $1; $1->push_back(*$3); delete $3; }
                        ;


exp:        pattern             { $$ = $1; }
        |   exp '+' exp	{ $$ = new FunctionCall( new FunctionCall(new Identifier("+"), $1) , $3); }
        |   exp '-' exp	{ $$ = new FunctionCall( new FunctionCall(new Identifier("-"), $1) , $3); }
        |   exp '/' exp         { $$ = new FunctionCall( new FunctionCall(new Identifier("/"), $1) , $3); }
        |   exp '*' exp	{ $$ = new FunctionCall( new FunctionCall(new Identifier("*"), $1) , $3); }
        |   exp '=' exp      { $$ = new FunctionCall( new FunctionCall(new Identifier("="), $1) , $3); }
        |   exp INFIX_OP exp      { $$ = new FunctionCall( new FunctionCall(new Identifier(*$2), $1) , $3); }
        |   PREFIX_OP exp         { $$ = new FunctionCall(new Identifier(*$1), $2); }
        |   exp exp   %prec FUNAPPLY    { $$ = new FunctionCall($1, $2); }
        |   let_statement IN exp   { $$ = new LetIn($1, $3); }
        |   IF exp THEN exp ELSE exp    { $$ = new Conditional($2, $4, $6); }
        |   FUNCTION IDENTIFIER INTO exp    { $$ = new Function(new Identifier(*$2), $4);}
        |   MATCH exp WITH patterns_and_cases   { $$ = new MatchWith($2, *$4); delete $4; }
        |   exp LIST_CONS exp   { $$ = new FunctionCall(new Identifier("Elem"), new TupleCreation(new std::vector<Expression*>{$1, $3})); }
        |   pattern LIST_CONS exp   { $$ = new FunctionCall(new Identifier("Elem"), new TupleCreation(new std::vector<Expression*>{$1, $3})); }
        |   '(' exp ')'   { $$ = $2; }
        ;

patterns_and_cases:     pattern INTO exp    { $$ = new vector<pair<Expression*, Expression*> >{ pair<Expression*,Expression*>($1, $3) }; }
                  |     patterns_and_cases '|' pattern INTO exp { $$ = $1; $$->push_back( pair<Expression*,Expression*>($3, $5) ); }

tuple:  '(' comma_separated_expressions ')'     { $$ = new TupleCreation($2); }
     ;

comma_separated_expressions:    exp ',' exp     { $$ = new vector<Expression*>{$1, $3}; }
                           |    comma_separated_expressions ',' exp     { $$ = $1; $$->push_back($3); }
                           ;

semicolon_separated_expressions:    exp     { $$ = new vector<Expression*>{$1}; }
                               |    semicolon_separated_expressions ';' exp     { $$ = $1; $$->push_back($3); }
                               ;

let_statement:  LET RECS pattern '=' exp  { $$ = new Let($3, $5, $2); }
             ;

RECS:   /* empty */ { $$ = false; }
        | REC       { $$ = true; }
        ;

pattern:    INTEGER_LITERAL	{ $$ = new Integer($1); }
        |   STRING_LITERAL	{ $$ = new String( *($1) ); }
        |   FLOAT_LITERAL	{ $$ = new Float($1); }
        |   VALUE_CONSTRUCTOR	{ $$ = new Identifier(*$1); }
        |   IDENTIFIER          { $$ = new Identifier( *$1 ); }
        |   BOOLEAN_LITERAL     { $$ = new Bool( $1 ); }
        |   VALUE_CONSTRUCTOR exp   %prec FUNAPPLY    { $$ = new FunctionCall(new Identifier(*$1), $2); delete $1; }
        |   '[' ']'             { $$ = new Identifier("End"); }
        |   pattern LIST_CONS pattern   { $$ = new FunctionCall(new Identifier("Elem"), new TupleCreation(new std::vector<Expression*>{$1, $3})); }
        |   '[' semicolon_separated_expressions ']'   { $$ = new Identifier("End"); for(auto it=$2->rbegin();it!=$2->rend();++it) $$ = new FunctionCall(new Identifier("Elem"), new TupleCreation(new std::vector<Expression*>{*it, $$})); }
        |   '(' pattern ')'     { $$ = $2; }
        |   '(' ')'             { $$ = new Identifier("Unit"); }
        |   tuple               { $$ = $1; }
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



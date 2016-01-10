/* occ */
/* occ.lex */

%{
#include "heading.h"
#include "tok.h"
int yyerror(char *s);
extern int yylineno;
#define YY_DECL extern "C" int yylex (void)
%}

digit		[0-9]
int_const	{digit}+
float_const {int_const}\.{int_const}*

small_letter      [a-z_]
big_letter        [A-Z]
any_letter        [a-z_A-Z]
ident             {small_letter}+{any_letter}*
string_const      \"{any_letter}*\"
value_constructor {big_letter}{any_letter}*
polymorphic_type  '{small_letter}+

%%

{int_const}		{ yylval.int_val = atoi(yytext); return INTEGER_LITERAL; }
{string_const}          { yylval.string_val = new std::string(yytext); return STRING_LITERAL; }
{float_const}           { yylval.float_val = atof(yytext); return FLOAT_LITERAL; }
{value_constructor}     { yylval.string_val = new std::string(yytext); return VALUE_CONSTRUCTOR; }
{polymorphic_type}      { yylval.string_val = new std::string(yytext); return POLYMORPHIC_TYPE; }
"+"                     { yylval.string_val = new std::string(yytext); return PLUS; }
"-"                     { yylval.string_val = new std::string(yytext); return MINUS; }
"*"			{ yylval.string_val = new std::string(yytext); return MULT; }
"/"                     { yylval.string_val = new std::string(yytext); return DIV; }
"="			{ yylval.string_val = new std::string(yytext); return EQUALS; }
"let"           { yylval.string_val = new std::string(yytext); return LET; }
"rec"		{ yylval.string_val = new std::string(yytext); return REC; }
"in"            { yylval.string_val = new std::string(yytext); return IN; }
"function"      { yylval.string_val = new std::string(yytext); return FUNCTION; }
"->"            { yylval.string_val = new std::string(yytext); return INTO; }
"if"            { yylval.string_val = new std::string(yytext); return IF; }
"then"            { yylval.string_val = new std::string(yytext); return THEN; }
"else"            { yylval.string_val = new std::string(yytext); return ELSE; }
"type"            { yylval.string_val = new std::string(yytext); return TYPE; }
"of"            { yylval.string_val = new std::string(yytext); return OF; }
";;"            { yylval.string_val = new std::string(yytext); return SEMIC2; }
[\(\)\{\}:;,|]   { return yytext[0]; }
{ident}         { yylval.string_val = new std::string(yytext); return IDENTIFIER; }

[ \t]*		{}
[\n]		{ yylineno++;}

.		{ yyerror("Unrecognized token");	}

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
any_letter        [a-z_A-Z]
ident       {small_letter}+{any_letter}*
string_const \"{any_letter}*\"

%%

{int_const}		{ yylval.int_val = atoi(yytext); return INTEGER_LITERAL; }
{string_const}   { yylval.string_val = new std::string(yytext); return STRING_LITERAL; }
{float_const}    { yylval.float_val = atof(yytext); return FLOAT_LITERAL; }
"+"				{ yylval.string_val = new std::string(yytext); return PLUS; }
"-"				{ yylval.string_val = new std::string(yytext); return MINUS; }
"*"				{ yylval.string_val = new std::string(yytext); return MULT; }
"/"				{ yylval.string_val = new std::string(yytext); return DIV; }
"="				{ yylval.string_val = new std::string(yytext); return EQUALS; }
"let"           { yylval.string_val = new std::string(yytext); return LET; }
"rec"			{ yylval.string_val = new std::string(yytext); return REC; }
"function"      { yylval.string_val = new std::string(yytext); return FUNCTION; }
"->"            { yylval.string_val = new std::string(yytext); return INTO; }
"if"            { yylval.string_val = new std::string(yytext); return IF; }
"then"            { yylval.string_val = new std::string(yytext); return THEN; }
"else"            { yylval.string_val = new std::string(yytext); return ELSE; }
";;"            { yylval.string_val = new std::string(yytext); return SEMIC2; }
[\(\)\{\}:;,]  { return yytext[0]; }
{ident}         { yylval.string_val = new std::string(yytext); return IDENTIFIER; }

[ \t]*		{}
[\n]		{ yylineno++;}

.		{ yyerror("Unrecognized token");	}

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
float_const {int_const}\.{int_const}

letter      [a-z_]
ident       {letter}+
string_const \"{ident}\"

%%

{int_const}		{ yylval.int_val = atoi(yytext); return INTEGER_LITERAL; }
{string_const}   { yylval.op_val = new std::string(yytext); return STRING_LITERAL; }
{float_const}    { yylval.float_val = atof(yytext); return FLOAT_LITERAL; }
"+"				{ yylval.op_val = new std::string(yytext); return PLUS; }
"-"				{ yylval.op_val = new std::string(yytext); return MINUS; }
"*"				{ yylval.op_val = new std::string(yytext); return MULT; }
"/"				{ yylval.op_val = new std::string(yytext); return DIV; }
"="				{ yylval.op_val = new std::string(yytext); return EQUALS; }
"let"           { yylval.op_val = new std::string(yytext); return LET; }
"rec"			{ yylval.op_val = new std::string(yytext); return REC; }
"function"      { yylval.op_val = new std::string(yytext); return FUNCTION; }
"->"            { yylval.op_val = new std::string(yytext); return INTO; }
"if"            { yylval.op_val = new std::string(yytext); return IF; }
"then"            { yylval.op_val = new std::string(yytext); return THEN; }
"else"            { yylval.op_val = new std::string(yytext); return ELSE; }
";;"            { yylval.op_val = new std::string(yytext); return SEMIC2; }
[\(\)\{\}:;,]  { return yytext[0]; }
{ident}         { yylval.op_val = new std::string(yytext); return IDENTIFIER; }

[ \t]*		{}
[\n]		{ yylineno++;	}

.		{ std::cerr << "SCANNER "; yyerror(""); exit(1);	}


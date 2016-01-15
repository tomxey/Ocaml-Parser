/* occ */
/* occ.lex */

%{
#include "heading.h"
#include "tok.h"
int yyerror(char *s);
extern int yylineno;
#define YY_DECL extern "C" int yylex (void)
%}

comment         \(\*[^\*]*\*\)

digit		[0-9]
int_const	{digit}+
float_const     {int_const}\.{int_const}*

ident             [a-z_][a-zA-Z0-9_]*
string_const      \"[^\"]*\"
value_constructor [A-Z][a-zA-Z0-9_]*
polymorphic_type  '[a-z]+

operator        [\!\$\%\&\*\+\-\.\/\:\<\=\>\?\@\^\|\~]
infix_op        [\=\<\>\@\^\|\&\+\-\*\/\$\%]+
prefix_op       [\!\?\~]+

%%

{comment}       { }
"true"           { yylval.bool_val = true; return BOOLEAN_LITERAL; }
"false"           { yylval.bool_val = false; return BOOLEAN_LITERAL; }
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
"match"            { yylval.string_val = new std::string(yytext); return MATCH; }
"with"            { yylval.string_val = new std::string(yytext); return WITH; }
";;"            { yylval.string_val = new std::string(yytext); return SEMIC2; }

{ident}         { yylval.string_val = new std::string(yytext); return IDENTIFIER; }

[-]?{int_const}		{ yylval.int_val = atoi(yytext); return INTEGER_LITERAL; }
{string_const}          { yylval.string_val = new std::string(yytext+1); yylval.string_val->resize(yylval.string_val->size()-1); return STRING_LITERAL; }
[-]?{float_const}           { yylval.float_val = atof(yytext); return FLOAT_LITERAL; }
{value_constructor}     { yylval.string_val = new std::string(yytext); return VALUE_CONSTRUCTOR; }
{polymorphic_type}      { yylval.string_val = new std::string(yytext); return POLYMORPHIC_TYPE; }

\([ ]*{operator}*[ ]*\)           { const char *begin, *end; begin = yytext + 1; while( *begin == ' ' ) begin++; end = begin; while( *end != ' ' && *end != ')' ) end++;
                                        yylval.string_val = new std::string(begin, end - begin); return IDENTIFIER;}
[\(\)\{\}:;,\|\+\-\*/=]   { return yytext[0]; }
{infix_op}{operator}*     { yylval.string_val = new std::string(yytext); return INFIX_OP; }
{prefix_op}{operator}*    { yylval.string_val = new std::string(yytext); return PREFIX_OP; }

[ \t]*		{}
[\n]		{ yylineno++;}

.		{ yyerror("Unrecognized token");	}

%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"

int yycolumn = 1;

#define YY_USER_ACTION                                   \
    yylloc.first_line   = yylineno;                      \
    yylloc.first_column = yycolumn;                      \
    yylloc.last_line    = yylineno;                      \
    yylloc.last_column  = yycolumn + yyleng - 1;         \
    yycolumn           += yyleng;
%}

%option noyywrap yylineno

%x COMMENT2 COMMENTLINEA

white_space       [ \t]+
digit             [0-9]
alpha             [A-Za-z_]
alpha_num         ({alpha}|{digit})
identifier        {alpha}{alpha_num}*
unsigned_integer  {digit}+
real              {digit}+\.{digit}+
string            \"([^"\n]|\"\")*\"

%%

<INITIAL>"//"      BEGIN(COMMENTLINEA);
<COMMENTLINEA>[^\n]* /* Ignorar todo hasta el final de la línea */
<COMMENTLINEA>\n      { BEGIN(INITIAL); yylineno++; yycolumn = 1; }

<INITIAL>"(*"      BEGIN(COMMENT2);
<COMMENT2>"*)"      BEGIN(INITIAL);
<COMMENT2>[^*)\n]+  /* Ignorar cualquier caracter que no sea *, ) o newline */
<COMMENT2>"*"       /* Ignorar un asterisco suelto */
<COMMENT2>")"       /* Ignorar un paréntesis suelto */
<COMMENT2>\n        { yylineno++; yycolumn = 1; }
<COMMENT2><<EOF>>   { yyerror("Comentario multi-línea no terminado"); BEGIN(INITIAL); }

if                  return(TOKEN_IF);
else                return(TOKEN_ELSE);
print               return(TOKEN_PRINT);
int                 return(TOKEN_DINT);
float               return(TOKEN_DFLOAT);
String              return(TOKEN_DSTRING);

">"                 return(TOKEN_MAYOR);
"<"                 return(TOKEN_MENOR);
"!"                 return(TOKEN_NEGACION);
"="                 return(TOKEN_IGUAL);

{unsigned_integer}  { yylval.string = strdup(yytext); return(TOKEN_UNSIGNED_INTEGER); }
{real}              { yylval.string = strdup(yytext); return(TOKEN_REAL); }
{string}            { yylval.string = strdup(yytext); return(TOKEN_STRING); }
{identifier}        { yylval.string = strdup(yytext); return(TOKEN_IDENTIFIER); }

[*/+\-,^()\[\]\{\}\;] { return(yytext[0]); }
{white_space}       { /* Ignorar espacios en blanco */ }
\n                  { yylineno++; yycolumn = 1; }
.                   { yyerror("Caracter ilegal"); }

%%


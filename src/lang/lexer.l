%option noyywrap
%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lang.h"
%}

%%

[[:space:]]+        ; 
"//".*              ;
[0-9]+(\.[0-9]+)?   { yylval.val = atof(yytext); return NUMBER; }

"+"                 { return PLUS; }
"-"                 { return MINUS; }
"*"                 { return TIMES; }
"/"                 { return DIVIDE; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }
";"                 { return SEMI; }

.                   { jl_set_error("Lexical error: unexpected char '%c'", yytext[0]); return 0; }

%%

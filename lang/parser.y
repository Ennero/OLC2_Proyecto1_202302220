%{
#include <stdio.h>
#include <stdlib.h>
#include "lang.h"   /* declara jl_set_error y jl_run/extern double jl_result en lang.h si lo deseas */

/* declarar extern para no definir jl_result aquí (la definición queda en lang.c) */
extern double jl_result;

void yyerror(const char *s);
int yylex(void);
%}


%union {
    double val;
}

%token <val> NUMBER
%token PLUS MINUS TIMES DIVIDE LPAREN RPAREN SEMI

%type <val> expr

%left PLUS MINUS
%left TIMES DIVIDE
%right UMINUS

%%

input:
    /* empty */
  | expr SEMI {
        jl_result = $1;
    }
  ;

expr:
    NUMBER            { $$ = $1; }
  | expr PLUS expr    { $$ = $1 + $3; }
  | expr MINUS expr   { $$ = $1 - $3; }
  | expr TIMES expr   { $$ = $1 * $3; }
  | expr DIVIDE expr  {
        if ($3 == 0) {
            jl_set_error("Runtime error: división por cero");
            $$ = 0;
        } else {
            $$ = $1 / $3;
        }
    }
  | MINUS expr %prec UMINUS { $$ = -$2; }
  | LPAREN expr RPAREN { $$ = $2; }
  ;

%%

void yyerror(const char *s) {
    jl_set_error("Syntax error: %s", s ? s : "parse error");
}

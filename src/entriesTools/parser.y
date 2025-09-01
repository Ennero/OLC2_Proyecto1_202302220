%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "ast/AbstractExpresion.h"

    extern int yylex(void);
    extern AbstractExpresion* ast_root;
    void yyerror(const char *s);

    extern char* yylasttext; 
%}

/* Inclusión de headers necesarios para las acciones semánticas */
%code requires {
    #include "ast/nodos/builders.h"
    #include "context/result.h"
    #include "context/bloque.h"
    #include "error_reporter.h"
    #include "ast/nodos/instrucciones/instruccion/reasignacion.h"
}

/* Habilitar el seguimiento de ubicaciones (línea/columna) */
%locations

/* Unión de tipos semánticos */
%union {
    char* string;
    AbstractExpresion* nodo;
    TipoDato tipoDato;
}

/* --- Declaración de Tokens --- */
%token TOKEN_IF TOKEN_ELSE TOKEN_WHILE TOKEN_FOR TOKEN_SWITCH TOKEN_CASE
%token TOKEN_BREAK TOKEN_CONTINUE TOKEN_RETURN TOKEN_PRINT TOKEN_MAIN TOKEN_CLASS
%token TOKEN_DINT TOKEN_DFLOAT TOKEN_DDOUBLE TOKEN_DBOOLEAN TOKEN_DCHAR TOKEN_DSTRING
%token <string> TOKEN_IDENTIFIER TOKEN_INTEGER TOKEN_HEX_INTEGER TOKEN_DOUBLE_LIT
%token <string> TOKEN_CHAR_LITERAL TOKEN_STRING_LITERAL TOKEN_TRUE TOKEN_FALSE TOKEN_FLOAT_LIT
%token TOKEN_IGUAL_IGUAL TOKEN_DIFERENTE TOKEN_MAYOR_IGUAL TOKEN_MENOR_IGUAL
%token TOKEN_AND TOKEN_OR TOKEN_INCREMENTO TOKEN_DECREMENTO

/* --- Tipos para No-Terminales --- */
%type <nodo> s lSentencia sentencia expr imprimir lista_Expr bloque declaracion_var primitivo reasignacion_var
%type <tipoDato> tipoPrimitivo

/* --- Precedencia y Asociatividad de Operadores --- */
%right '='
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_IGUAL_IGUAL TOKEN_DIFERENTE
%left '<' '>' TOKEN_MAYOR_IGUAL TOKEN_MENOR_IGUAL
%left '+' '-'
%left '*' '/' '%'
%right '!' NEG

%%

%start s;

s: lSentencia  { ast_root = $1; $$ = $1; }
    ;

lSentencia: lSentencia sentencia { 
        // Si la sentencia fue un error, $2 será NULL.
        // Esta comprobación evita que un error corrompa el AST.
        if ($2 != NULL) {
            agregarHijo($1, $2);
        }
        $$ = $1;
    }
    | sentencia {
        AbstractExpresion* b = nuevoInstruccionesExpresion();
        // También se comprueba aquí para la primera sentencia.
        if ($1 != NULL) {
            agregarHijo(b, $1);
        }
        $$ = b;
    }
    ;

sentencia: imprimir ';'        { $$ = $1; }
    | bloque               { $$ = $1; }
    | declaracion_var ';'  { $$ = $1; }
    | reasignacion_var ';'  { $$ = $1; }
    | error ';' { 
        yyerrok; 
        $$ = NULL; 
    }
    ;

lista_Expr: lista_Expr ',' expr { agregarHijo($1, $3); $$ = $1; }
    | expr { 
        AbstractExpresion* b = nuevoListaExpresiones();
        agregarHijo(b, $1);
        $$ = b;
    }
    ;

imprimir: TOKEN_PRINT '(' lista_Expr ')' { $$ = nuevoPrintExpresion($3); }
    ;

bloque: '{' lSentencia '}' { $$ = nuevoBloqueExpresion($2); }
    | '{' '}' { $$ = nuevoBloqueExpresion(NULL); }
    ;

declaracion_var: tipoPrimitivo TOKEN_IDENTIFIER { $$ = nuevoDeclaracionVariables($1, $2, NULL, @2.first_line, @2.first_column); }
    | tipoPrimitivo TOKEN_IDENTIFIER '=' expr { $$ = nuevoDeclaracionVariables($1, $2, $4, @2.first_line, @2.first_column); }
    ;

reasignacion_var: TOKEN_IDENTIFIER '=' expr { $$ = nuevoReasignacionExpresion($1, $3, @1.first_line, @1.first_column); }
    ;

expr: expr '+' expr   { $$ = nuevoSumaExpresion($1, $3); }
    | expr '-' expr { $$ = nuevoRestaExpresion($1, $3); }
    | expr '*' expr   { $$ = nuevoMultiplicacionExpresion($1, $3); }
    | expr '/' expr   { $$ = nuevoDivisionExpresion($1, $3); }
    | expr '%' expr   { $$ = nuevoModuloExpresion($1, $3); }
    | '(' expr ')' { $$ = $2; }
    | '-' expr %prec NEG  { $$ = nuevoUnarioExpresion($2); }
    | primitivo { $$ = $1; }
    | TOKEN_IDENTIFIER { $$ = nuevoIdentificadorExpresion($1, @1.first_line, @1.first_column); }
    ;

primitivo: TOKEN_INTEGER        { $$ = nuevoPrimitivoExpresion($1, INT); }
    | TOKEN_HEX_INTEGER    { $$ = nuevoPrimitivoExpresion($1, INT); }
    | TOKEN_FLOAT_LIT      { $$ = nuevoPrimitivoExpresion($1, FLOAT); }
    | TOKEN_DOUBLE_LIT     { $$ = nuevoPrimitivoExpresion($1, DOUBLE); }
    | TOKEN_STRING_LITERAL { $$ = nuevoPrimitivoExpresion($1, STRING); }
    | TOKEN_CHAR_LITERAL   { $$ = nuevoPrimitivoExpresion($1, CHAR); }
    | TOKEN_TRUE           { $$ = nuevoPrimitivoExpresion($1, BOOLEAN); }
    | TOKEN_FALSE          { $$ = nuevoPrimitivoExpresion($1, BOOLEAN); }
    ;

tipoPrimitivo: TOKEN_DINT     { $$ = INT; }
    | TOKEN_DFLOAT    { $$ = FLOAT; }
    | TOKEN_DDOUBLE   { $$ = DOUBLE; }
    | TOKEN_DSTRING   { $$ = STRING; }
    | TOKEN_DCHAR     { $$ = CHAR; }
    | TOKEN_DBOOLEAN  { $$ = BOOLEAN; }
    ;

%%

void yyerror(const char *s) {
    add_error_to_report("Sintáctico", yylasttext, s, yylloc.first_line, yylloc.first_column);
}

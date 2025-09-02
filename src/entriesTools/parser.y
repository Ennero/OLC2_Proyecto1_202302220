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
    #include "ast/nodos/instrucciones/instruccion/casteos.h"
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
%token TOKEN_LSHIFT TOKEN_RSHIFT TOKEN_URSHIFT 
%token TOKEN_PLUS_ASSIGN TOKEN_MINUS_ASSIGN TOKEN_MULT_ASSIGN TOKEN_DIV_ASSIGN TOKEN_MOD_ASSIGN
%token TOKEN_AND_ASSIGN TOKEN_OR_ASSIGN TOKEN_XOR_ASSIGN
%token TOKEN_LSHIFT_ASSIGN TOKEN_RSHIFT_ASSIGN TOKEN_URSHIFT_ASSIGN
%token TOKEN_DOT


/* --- Tipos para No-Terminales --- */
%type <nodo> s lSentencia sentencia expr imprimir lista_Expr bloque declaracion_var primitivo reasignacion_var primary_expr
%type <tipoDato> tipoPrimitivo

/* --- Precedencia y Asociatividad de Operadores (MODIFICADO) --- */
%left TOKEN_DOT
%right '=' TOKEN_PLUS_ASSIGN TOKEN_MINUS_ASSIGN TOKEN_MULT_ASSIGN TOKEN_DIV_ASSIGN TOKEN_MOD_ASSIGN TOKEN_AND_ASSIGN TOKEN_OR_ASSIGN TOKEN_XOR_ASSIGN TOKEN_LSHIFT_ASSIGN TOKEN_RSHIFT_ASSIGN TOKEN_URSHIFT_ASSIGN
%left TOKEN_OR
%left TOKEN_AND
%left '|'
%left '^'
%left '&'
%left TOKEN_IGUAL_IGUAL TOKEN_DIFERENTE
%left '<' '>' TOKEN_MAYOR_IGUAL TOKEN_MENOR_IGUAL
%left TOKEN_LSHIFT TOKEN_RSHIFT TOKEN_URSHIFT 
%left '+' '-'
%left '*' '/' '%'
%right '!' '~' NEG  // NEG es el alias para la negación unaria
%right '(' ')'

%%

%start s;

s: lSentencia  { ast_root = $1; $$ = $1; }
    ;

lSentencia: lSentencia sentencia { 
        // Si la sentencia fue un error, $2 será NULL.
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

imprimir: TOKEN_PRINT '(' lista_Expr ')' { $$ = nuevoPrintExpresion($3, @1.first_line, @1.first_column); }
    ;

bloque: '{' lSentencia '}' { $$ = nuevoBloqueExpresion($2, @1.first_line, @1.first_column); }
    | '{' '}' { $$ = nuevoBloqueExpresion(NULL, @1.first_line, @1.first_column); }
    ;

declaracion_var: tipoPrimitivo TOKEN_IDENTIFIER { $$ = nuevoDeclaracionVariables($1, $2, NULL, @2.first_line, @2.first_column); }
    | tipoPrimitivo TOKEN_IDENTIFIER '=' expr { $$ = nuevoDeclaracionVariables($1, $2, $4, @2.first_line, @2.first_column); }
    ;

reasignacion_var: TOKEN_IDENTIFIER '=' expr { $$ = nuevoReasignacionExpresion($1, $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_PLUS_ASSIGN expr  { $$ = nuevoAsignacionCompuestaExpresion($1, '+', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_MINUS_ASSIGN expr { $$ = nuevoAsignacionCompuestaExpresion($1, '-', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_MULT_ASSIGN expr  { $$ = nuevoAsignacionCompuestaExpresion($1, '*', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_DIV_ASSIGN expr   { $$ = nuevoAsignacionCompuestaExpresion($1, '/', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_MOD_ASSIGN expr   { $$ = nuevoAsignacionCompuestaExpresion($1, '%', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_AND_ASSIGN expr    { $$ = nuevoAsignacionCompuestaExpresion($1, '&', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_OR_ASSIGN expr     { $$ = nuevoAsignacionCompuestaExpresion($1, '|', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_XOR_ASSIGN expr     { $$ = nuevoAsignacionCompuestaExpresion($1, '^', $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_LSHIFT_ASSIGN expr  { $$ = nuevoAsignacionCompuestaExpresion($1, TOKEN_LSHIFT, $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_RSHIFT_ASSIGN expr  { $$ = nuevoAsignacionCompuestaExpresion($1, TOKEN_RSHIFT, $3, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER TOKEN_URSHIFT_ASSIGN expr { $$ = nuevoAsignacionCompuestaExpresion($1, TOKEN_URSHIFT, $3, @1.first_line, @1.first_column); }
    ;



    expr: expr '+' expr                  { $$ = nuevoSumaExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '-' expr                { $$ = nuevoRestaExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '*' expr                { $$ = nuevoMultiplicacionExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '/' expr                { $$ = nuevoDivisionExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '%' expr                { $$ = nuevoModuloExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_LSHIFT expr       { $$ = nuevoLeftShiftExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_RSHIFT expr       { $$ = nuevoRightShiftExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_URSHIFT expr      { $$ = nuevoUnsignedRightShiftExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '&' expr                { $$ = nuevoBitwiseAndExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '|' expr                { $$ = nuevoBitwiseOrExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '^' expr                { $$ = nuevoBitwiseXorExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_IGUAL_IGUAL expr  { $$ = nuevoIgualExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_DIFERENTE expr    { $$ = nuevoDiferenteExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '<' expr                { $$ = nuevoMenorQueExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr '>' expr                { $$ = nuevoMayorQueExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_MENOR_IGUAL expr  { $$ = nuevoMenorIgualExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_MAYOR_IGUAL expr  { $$ = nuevoMayorIgualExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_AND expr          { $$ = nuevoAndExpresion($1, $3, @2.first_line, @2.first_column); }
    | expr TOKEN_OR expr           { $$ = nuevoOrExpresion($1, $3, @2.first_line, @2.first_column); }
    | primary_expr                 { $$ = $1; } // Una expresión puede ser una expresión primaria
    ;

// NUEVA REGLA PARA EXPRESIONES PRIMARIAS (las de más alta precedencia)
primary_expr:
      '(' expr ')'                 { $$ = $2; }
    | '(' tipoPrimitivo ')' expr   { $$ = nuevoCasteoExpresion($2, $4, @1.first_line, @1.first_column); }
    | TOKEN_IDENTIFIER             { $$ = nuevoIdentificadorExpresion($1, @1.first_line, @1.first_column); }
    | primitivo                    { $$ = $1; }
    | '-' primary_expr %prec NEG   { $$ = nuevoUnarioExpresion($2, @1.first_line, @1.first_column); }
    | '!' primary_expr             { $$ = nuevoNotExpresion($2, @1.first_line, @1.first_column); }
    | '~' primary_expr             { $$ = nuevoBitwiseNotExpresion($2, @1.first_line, @1.first_column); }
    // La llamada a método ahora solo puede ocurrir sobre una expresión primaria
    | primary_expr TOKEN_DOT TOKEN_IDENTIFIER '(' expr ')' {
        if (strcmp($3, "equals") == 0) {
            $$ = nuevoEqualsExpresion($1, $5, @2.first_line, @2.first_column);
        } else {
            char desc[100];
            sprintf(desc, "El método '%s' no está definido.", $3);
            yyerror(desc);
            $$ = NULL;
        }
        free($3);
    }
    ;

primitivo: TOKEN_INTEGER        { $$ = nuevoPrimitivoExpresion($1, INT, @1.first_line, @1.first_column); }
    | TOKEN_HEX_INTEGER    { $$ = nuevoPrimitivoExpresion($1, INT, @1.first_line, @1.first_column); }
    | TOKEN_FLOAT_LIT      { $$ = nuevoPrimitivoExpresion($1, FLOAT, @1.first_line, @1.first_column); }
    | TOKEN_DOUBLE_LIT     { $$ = nuevoPrimitivoExpresion($1, DOUBLE, @1.first_line, @1.first_column); }
    | TOKEN_STRING_LITERAL { $$ = nuevoPrimitivoExpresion($1, STRING, @1.first_line, @1.first_column); }
    | TOKEN_CHAR_LITERAL   { $$ = nuevoPrimitivoExpresion($1, CHAR, @1.first_line, @1.first_column); }
    | TOKEN_TRUE           { $$ = nuevoPrimitivoExpresion($1, BOOLEAN, @1.first_line, @1.first_column); }
    | TOKEN_FALSE          { $$ = nuevoPrimitivoExpresion($1, BOOLEAN, @1.first_line, @1.first_column); }
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
    add_error_to_report("Sintáctico", yylasttext, s, yylloc.first_line, yylloc.first_column, NULL);
}

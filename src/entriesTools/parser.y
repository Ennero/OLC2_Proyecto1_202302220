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
%token TOKEN_PUBLIC TOKEN_STATIC TOKEN_VOID
%token TOKEN_IF TOKEN_ELSE TOKEN_WHILE TOKEN_FOR TOKEN_SWITCH TOKEN_CASE
%token TOKEN_BREAK TOKEN_CONTINUE TOKEN_RETURN TOKEN_PRINT TOKEN_MAIN
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
%token TOKEN_DEFAULT



/* --- Tipos para No-Terminales --- */
%type <nodo> s lSentencia sentencia expr lista_Expr bloque declaracion_var primitivo reasignacion_var primary_expr if_stmt switch_stmt case_list case_stmt lSentencia_opt programa lista_declaraciones declaracion declaracion_funcion lista_parametros_opt lista_parametros parametro lista_argumentos_opt declaracion_main while_stmt for_stmt for_init for_expr_opt
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
%start programa;

programa: lista_declaraciones { ast_root = $1; $$ = $1; };

lista_declaraciones: lista_declaraciones declaracion { agregarHijo($1, $2); $$ = $1; }
                    | declaracion { AbstractExpresion* lista = nuevoInstruccionesExpresion(); agregarHijo(lista, $1); $$ = lista; };

declaracion: declaracion_funcion { $$ = $1; }
            | declaracion_main    { $$ = $1; }
            | declaracion_var ';' { $$ = $1; }
            ;

declaracion_funcion: tipoPrimitivo TOKEN_IDENTIFIER '(' lista_parametros_opt ')' bloque {
                        $$ = nuevoDeclaracionFuncion($1, $2, $4, $6, @1.first_line, @1.first_column);
                        };

lista_parametros_opt: lista_parametros { $$ = $1; }
                    | %empty { $$ = nuevoListaExpresiones(); };

// <<<< REGLA AÑADIDA: Para reconocer uno o más parámetros separados por comas >>>>
lista_parametros: lista_parametros ',' parametro { agregarHijo($1, $3); $$ = $1; }
                | parametro { AbstractExpresion* lista = nuevoListaExpresiones(); agregarHijo(lista, $1); $$ = lista; };

parametro: tipoPrimitivo TOKEN_IDENTIFIER { $$ = nuevoParametro($1, $2); };


// Regla para la sintaxis "public static void main(String[] args) { ... }"
declaracion_main: TOKEN_PUBLIC TOKEN_STATIC TOKEN_VOID TOKEN_MAIN 
                    '(' TOKEN_DSTRING '[' ']' TOKEN_IDENTIFIER ')' bloque {
                        $$ = nuevoMainFunctionNode($11, @1.first_line, @1.first_column);
                    }
                ;


// <<<< REGLA AÑADIDA: Para argumentos en una llamada a función >>>>
lista_argumentos_opt: lista_Expr { $$ = $1; }
                    | %empty   { $$ = NULL; };


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

sentencia: bloque           { $$ = $1; }
    | declaracion_var ';'   { $$ = $1; }
    | if_stmt               { $$ = $1; }
    | while_stmt            { $$ = $1; }
    | for_stmt              { $$ = $1; }
    | switch_stmt           { $$ = $1; }
    | TOKEN_BREAK ';'       { $$ = nuevoBreakExpresion(@1.first_line, @1.first_column); }
    | TOKEN_CONTINUE ';'    { $$ = nuevoContinueExpresion(@1.first_line, @1.first_column); }
    | TOKEN_RETURN expr ';' { $$ = nuevoReturnExpresion($2, @1.first_line, @1.first_column); }
    | TOKEN_RETURN ';'      { $$ = nuevoReturnExpresion(NULL, @1.first_line, @1.first_column); } 
    | expr ';'              { $$ = $1; }
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


expr: reasignacion_var             { $$ = $1; }
    | expr '+' expr                { $$ = nuevoSumaExpresion($1, $3, @2.first_line, @2.first_column); }
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
    // <<<< REGLA AÑADIDA: Para reconocer una llamada a función como una expresión >>>>
    | TOKEN_IDENTIFIER '(' lista_argumentos_opt ')' { $$ = nuevoLlamadaFuncion($1, $3, @1.first_line, @1.first_column); }
    | TOKEN_PRINT '(' lista_Expr ')' { $$ = nuevoPrintExpresion($3, @1.first_line, @1.first_column); }

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




if_stmt: TOKEN_IF '(' expr ')' bloque { 
            // Regla para un if simple: if (cond) { ... }
            $$ = nuevoIfExpresion($3, $5, NULL, @1.first_line, @1.first_column);
        }
    | TOKEN_IF '(' expr ')' bloque TOKEN_ELSE bloque {
            // Regla para un if-else: if (cond) { ... } else { ... }
            $$ = nuevoIfExpresion($3, $5, $7, @1.first_line, @1.first_column);
        }
    | TOKEN_IF '(' expr ')' bloque TOKEN_ELSE if_stmt {
            // Regla para un else-if: if (cond) { ... } else if (...)
            $$ = nuevoIfExpresion($3, $5, $7, @1.first_line, @1.first_column);
        }
    ;


// 3. AÑADE ESTA NUEVA REGLA COMPLETA
while_stmt: TOKEN_WHILE '(' expr ')' bloque {
                $$ = nuevoWhileExpresion($3, $5, @1.first_line, @1.first_column);
            };



for_init: declaracion_var { $$ = $1; }
        | expr            { $$ = $1; };

// Regla para la condición y actualización (puede ser expresión o nada)
for_expr_opt: expr            { $$ = $1; }
            | %empty          { $$ = NULL; };

// Regla principal para la sentencia for
for_stmt: TOKEN_FOR '(' for_init ';' for_expr_opt ';' for_expr_opt ')' bloque {
                $$ = nuevoForExpresion($3, $5, $7, $9, @1.first_line, @1.first_column);
            };

switch_stmt: TOKEN_SWITCH '(' expr ')' '{' case_list '}' {
                $$ = nuevoSwitchExpresion($3, $6, @1.first_line, @1.first_column);
            };





// Regla para una lista de uno o más casos (case o default)
case_list: case_list case_stmt { agregarHijo($1, $2); $$ = $1; }
        | case_stmt         { 
                                AbstractExpresion* lista = nuevoListaExpresiones();
                                agregarHijo(lista, $1);
                                $$ = lista;
                            }
        ;

// Regla para un caso individual, ya sea 'case' o 'default'
case_stmt: TOKEN_CASE expr ':' lSentencia_opt { 
               // Un 'case' con una expresión y una lista OPCIONAL de sentencias
            $$ = nuevoCaseExpresion($2, $4, @1.first_line, @1.first_column);
            }
        | TOKEN_DEFAULT ':' lSentencia_opt {
               // El caso 'default' con una lista OPCIONAL de sentencias
                $$ = nuevoDefaultExpresion($3, @1.first_line, @1.first_column);
        }
        ;


lSentencia_opt: lSentencia { $$ = $1; } // Si hay sentencias, las usamos.
              | %empty   { $$ = NULL; } // Si no hay nada, devolvemos NULL.
            ;


%%

void yyerror(const char *s) {
    add_error_to_report("Sintáctico", yylasttext, s, yylloc.first_line, yylloc.first_column, NULL);
}

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "ast/AbstractExpresion.h"

    /* Prototipo del scanner */
    extern int yylex(void);
    extern AbstractExpresion* ast_root;
    /* yyerror con firma estándar */
    void yyerror(const char *s);
%}

/* Esto va al parser.tab.h */
%code requires {
    #include "ast/nodos/builders.h"
    #include "context/result.h"
}

/* Seguimiento de ubicaciones */
%locations
/* Mensajes de error más detallados */
//%define parse.error verbose

/* Unión de tipos semánticos */
%union {
    char* string;
    AbstractExpresion* nodo;
}




/* TOKENS SIN VALOR (PALABRAS CLAVE)                   */
/* --- Tipos de Dato --- */
%token RW_INT RW_FLOAT RW_STRING RW_BOOLEAN RW_CHAR RW_VOID

/* --- Control de Flujo --- */
%token RW_IF RW_ELSE RW_FOR RW_WHILE RW_DO RW_SWITCH RW_CASE RW_DEFAULT

/* --- Transferencia --- */
%token RW_BREAK RW_CONTINUE RW_RETURN

/* --- Modificadores y Otros --- */
%token RW_PRINT RW_FINAL RW_PUBLIC RW_STATIC

/* --- Operadores de Múltiples Caracteres --- */
%token RW_AND RW_OR RW_IGUAL RW_NO_IGUAL RW_MAYOR_IGUAL RW_MENOR_IGUAL
%token RW_MAS_IGUAL RW_INCREMENTO RW_DECREMENTO RW_MENOS_IGUAL
%token RW_DIVISION_IGUAL RW_MULTIPLICACION_IGUAL RW_MODULO_IGUAL
%token RW_Y_LOGICO_IGUAL RW_O_LOGICO_IGUAL RW_EXP_IGUAL
%token RW_DESPLAZAMIENTO_IZQUIERDO_IGUAL RW_DESPLAZAMIENTO_DERECHO_IGUAL

/* --- Operadores y Puntuación de Un Carácter --- */
%token RW_IGUALAR RW_PUNTO_Y_COMA RW_COMA RW_PUNTO RW_DOS_PUNTOS
%token RW_ABRE_PARENTESIS RW_CIERRA_PARENTESIS
%token RW_ABRE_CORCHETE RW_CIERRA_CORCHETE
%token RW_ABRE_LLAVE RW_CIERRA_LLAVE
%token RW_MAYOR RW_MENOR RW_NEGACION RW_MAS RW_MENOS
%token RW_MULTIPLICACION RW_DIVISION RW_MODULO

/* TOKENS CON VALOR <string> (Literales e Identificadores)         */
%token <string> TK_INTEGER TK_REAL TK_STRING TK_CHAR TK_BOOLEAN TK_NULL TK_IDENTIFIER


/* DEFINICIÓN DE TIPOS                          */
/* Tipo de los no-terminales que llevan valor (esto se mantiene igual) */
%type <nodo> s lSentencia sentencia expr imprimir lista_Expr bloque declaracion_var asignacion_var primitivo


// precedencia menor a mayor
//%left NUMERO
%right RW_IGUALAR RW_MAS_IGUAL RW_MENOS_IGUAL RW_MULTIPLICACION_IGUAL RW_DIVISION_IGUAL RW_MODULO_IGUAL /* Operadores de asignación */
%left RW_OR            /* || */
%left RW_AND           /* && */
%left RW_IGUAL RW_NO_IGUAL /* == != */
%left RW_MAYOR RW_MENOR RW_MAYOR_IGUAL RW_MENOR_IGUAL /* > < >= <= */
%left RW_MAS RW_MENOS   /* + - (Binarios) */
%left RW_MULTIPLICACION RW_DIVISION RW_MODULO /* * / % */
%right RW_NEGACION     /* ! (Not Lógico) */
%right NEG          /* - (Negación unaria */
%right RW_INCREMENTO RW_DECREMENTO /* ++ --  */

%%

%start s;

s: lSentencia  { ast_root = $1; $$ = $1; }
    //| error '\n'  { yyerrok; }
    ;
//                                               Padre, hijo;
lSentencia: lSentencia sentencia { agregarHijo($1, $2); $$ = $1;} |
            sentencia {
                        AbstractExpresion* b = nuevoInstruccionesExpresion();
                        agregarHijo(b, $1);
                        $$ =  b;
                        }
    ;

sentencia: imprimir RW_PUNTO_Y_COMA {$$ = $1; } |
        bloque {$$ = $1;}
    //| asignacion_var {$$ = $1; }
    //| declaracion_var {$$ = $1;}
    ;

lista_Expr: lista_Expr RW_COMA expr { agregarHijo($1, $3); $$ = $1; }
    | expr {
                AbstractExpresion* b = nuevoListaExpresiones();
                agregarHijo(b, $1);
                $$ =  b;
            }
    ;

imprimir: RW_PRINT RW_ABRE_PARENTESIS lista_Expr RW_CIERRA_PARENTESIS { $$ =  nuevoPrintExpresion($3); }
    ;

bloque: RW_ABRE_LLAVE lSentencia RW_CIERRA_LLAVE { AbstractExpresion* b = nuevoInstruccionesExpresion();
                        agregarHijo(b, $2);
                        $$ =  b; }


/* declaracion_var: primitivo {   T_ID* id = new T_ID(QString::fromStdString($2));
                                    $$ = new NT_DeclVar($4, id); }
    | primitivo '=' expr { T_ID* id = new T_ID(QString::fromStdString($2));
                                $$ = new NT_DeclVar($4, id,$6 );  }
    ;

asignacion_var: TOKEN_IDENTIFIER '=' expr   {   
                                                T_ID* id_avar = new T_ID(QString::fromStdString($1));
                                                $$ = new NT_AsigVar(id_avar, $3);
                                            }
    ; */


//TODO: para mejorar la legibilidad en lugar de guardar la operacion por incumplir el principio de responsabilidad única y también el Principio de abierto/cerrado (SOLID)
// la accion semantica fungirá como factoría abstracta y cada expresión tendra su método interpret para hacer la operación además de utilizar otros métodos "heredados".
/* expr: expr '+' expr   { $$ =  nuevoExpresionLenguaje('+', $1, $3);  }
    | expr '-' expr { $$ =  nuevoExpresionLenguaje('-', $1, $3); }
    | '(' expr ')' { $$ = $2; }
    | '-' expr %prec NEG  { $$ =  nuevoExpresionLenguaje('U', $2, NULL);  }
    | primitivo { $$ = $1; }
    // | TOKEN_IDENTIFIER { $$ =  nuevoExpresionLenguaje('I', $1, $3); }
    ; */

// El corazón de tu lenguaje: maneja todas las operaciones
expr: expr RW_MAS expr                { $$ = nuevoSumaExpresion($1, $3); }
    | expr RW_MENOS expr              { $$ = nuevoRestaExpresion($1, $3); }
    | expr RW_MULTIPLICACION expr     { /* Llama a tu función para multiplicación */ }
    | expr RW_DIVISION expr           { /* Llama a tu función para división */ }
    | expr RW_MODULO expr             { /* Llama a tu función para módulo */ }
    | expr RW_AND expr                { /* Llama a tu función para AND lógico */ }
    | expr RW_OR expr                 { /* Llama a tu función para OR lógico */ }
    | expr RW_IGUAL expr              { /* Llama a tu función para == */ }
    | expr RW_NO_IGUAL expr           { /* Llama a tu función para != */ }
    | expr RW_MAYOR expr              { /* Llama a tu función para > */ }
    | expr RW_MENOR expr              { /* Llama a tu función para < */ }
    | expr RW_MAYOR_IGUAL expr        { /* Llama a tu función para >= */ }
    | expr RW_MENOR_IGUAL expr        { /* Llama a tu función para <= */ }
    | RW_NEGACION expr                { /* Llama a tu función para NOT lógico ! */ }
    | RW_MENOS expr %prec NEG         { $$ = nuevoUnarioExpresion($2); }
    | TK_IDENTIFIER                   { /* $$ = nuevoVariableExpresion($1); */ }
    | primitivo                       { $$ = $1; }
    | RW_ABRE_PARENTESIS expr RW_CIERRA_PARENTESIS { $$ = $2; }
    ;

primitivo: TK_INTEGER { $$ =  nuevoPrimitivoExpresion($1, INT); } |
            TK_REAL { $$ =  nuevoPrimitivoExpresion($1, FLOAT); } |
            TK_CHAR { $$ =  nuevoPrimitivoExpresion($1, CHAR); } |
            TK_STRING { $$ =  nuevoPrimitivoExpresion($1, STRING); } |
            TK_BOOLEAN { $$ =  nuevoPrimitivoExpresion($1, BOOLEAN); } |
            TK_NULL { $$ =  nuevoPrimitivoExpresion($1, NULL); }
    ;
%%

/* definición de yyerror, usa el yylloc global para ubicación */
void yyerror(const char *s) {
    fprintf(stderr,
            "Illegal input %s en %d:%d\n",
            s,
            yylloc.first_line,
            yylloc.first_column);
}

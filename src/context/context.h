#ifndef CONTEXT_H
#define CONTEXT_H

#include "result.h"
// Tipos y estructuras del sistema de contextos (ámbitos) y símbolos.
#include <stddef.h> // Define el tipo 'size_t'

typedef struct AbstractExpresion AbstractExpresion; 


typedef enum {
    VARIABLE,
    FUNCION,
    STRUCT,
} Clase;

typedef struct {
    TipoDato tipo;     // Tipo base (INT, STRING, ...)
    int dimensiones;   // 0 si escalar; >0 si arreglo (total de dimensiones)
    char* nombre;
} Parametro;

typedef struct {
    void* valor;   // Valor de la variable
    int borrowed;  // 1 si el valor es prestado (no liberar en este contexto)
} VariableInfo;

typedef struct {
    TipoDato tipo_retorno;
    int retorno_dimensiones; // 0 si escalar; >0 si retorna arreglo
    Parametro* parametros;
    size_t num_parametros;
    AbstractExpresion* cuerpo; // Puntero al AST del bloque de la función
} FuncionInfo;

typedef struct Symbol Symbol;
typedef struct Context Context;

struct Symbol{
    char* nombre;
    Clase clase; 
    TipoDato tipo; // Para var: su tipo. Para func: su tipo de retorno.
    // Nota: el valor se encuentra dentro de la unión 'info'
    union {
        VariableInfo var;
        FuncionInfo func;
    } info;
    int es_constante; // 1 si fue declarada con 'final'
    int line;
    int column;
    Symbol* anterior;
};

struct Context {
    char* nombre_completo; 
    Context* anterior;
    Symbol* ultimoSymbol;
    Context* raiz;         // Puntero al contexto raíz (global)
    int proximo_id_bloque; // Contador utilizado por la raíz (si se requiere)
};

// Se actualizan las firmas
Context* nuevoContext(Context* anterior, const char* tipo_contexto);
void liberarContext(Context* context); // Libera el contexto y sus símbolos
Symbol* nuevoVariable(char* nombre, void* valor, TipoDato tipo, int es_constante);
void agregarSymbol(Context* actual, Symbol* symbol, int line, int column);
Symbol* buscarSymbol(Symbol* actual, char* nombre);
Symbol* buscarTablaSimbolos(Context* actual, char* nombre);

#endif // CONTEXT_H
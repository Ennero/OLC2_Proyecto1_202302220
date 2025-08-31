#ifndef CONTEXT_H
#define CONTEXT_H

#include "result.h"

typedef enum {
    VARIABLE,
    FUNCION,
    STRUCT,
} Clase;

typedef struct Symbol Symbol;
typedef struct Context Context;

struct Symbol{
    char* nombre;
    void* valor;
    TipoDato tipo;
    Clase clase;
    int line;
    int column;
    Symbol* anterior;
};

struct Context {
    char* nombre_completo; // <<<<<<< CAMBIO: de int a char*
    Context* anterior;
    Symbol* ultimoSymbol;
};

// Se actualizan las firmas
Context* nuevoContext(Context* anterior, const char* tipo_contexto);
Symbol* nuevoVariable(char* nombre, void* valor, TipoDato tipo);
void agregarSymbol(Context* actual, Symbol* symbol, int line, int column);
Symbol* buscarSymbol(Symbol* actual, char* nombre);
Symbol* buscarTablaSimbolos(Context* actual, char* nombre);

#endif // CONTEXT_H
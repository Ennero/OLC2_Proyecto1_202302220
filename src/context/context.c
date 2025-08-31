#include "context.h"
#include "symbol_reporter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// La función ahora construye el nombre completo del contexto
Context* nuevoContext(Context* anterior, const char* tipo_contexto) {
    Context* nuevo = malloc(sizeof(Context));
    nuevo->anterior = anterior;
    nuevo->ultimoSymbol = NULL;

    if (anterior && anterior->nombre_completo) {
        // Concatenar: nombre_padre + "_" + tipo_contexto
        size_t len = strlen(anterior->nombre_completo) + strlen(tipo_contexto) + 2; // +2 para '_' y '\0'
        nuevo->nombre_completo = malloc(len);
        snprintf(nuevo->nombre_completo, len, "%s_%s", anterior->nombre_completo, tipo_contexto);
    } else {
        // Si es el primer contexto (global)
        nuevo->nombre_completo = strdup(tipo_contexto);
    }

    return nuevo;
}

// ... (nuevoVariable no cambia) ...
Symbol* nuevoVariable(char* nombre, void* valor, TipoDato tipo) {
    Symbol* nuevo = malloc(sizeof(Symbol));
    nuevo->nombre = nombre;
    nuevo->valor = valor;
    nuevo->tipo = tipo;
    nuevo->clase = VARIABLE;
    nuevo->line = 0;
    nuevo->column = 0;
    return nuevo;
}


// Se actualiza la llamada al reportero para pasar el nombre del contexto
void agregarSymbol(Context* actual, Symbol* symbol, int line, int column) {
    if (buscarSymbol(actual->ultimoSymbol, symbol->nombre)) {
        printf("La variable de nombre '%s' ya existe.\n", symbol->nombre);
        return;
    }
    
    symbol->line = line;
    symbol->column = column;

    // <<<<<<< CAMBIO: Pasamos el nombre completo del contexto actual
    add_symbol_to_report(symbol, actual->nombre_completo);

    symbol->anterior = actual->ultimoSymbol;
    actual->ultimoSymbol = symbol;
}

// ... (buscarSymbol y buscarTablaSimbolos no cambian) ...
Symbol* buscarSymbol(Symbol* actual, char* nombre) {
    while (actual) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->anterior;
    }
    return NULL;
}

Symbol* buscarTablaSimbolos(Context* actual, char* nombre) {
    while (actual) {
        Symbol* symbolEncontrado = buscarSymbol(actual->ultimoSymbol, nombre);
        if (symbolEncontrado) {
            return symbolEncontrado;
        }
        actual = actual->anterior;
    }
    return NULL;
}

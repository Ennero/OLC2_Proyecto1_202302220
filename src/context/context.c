#include "context.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbol_reporter.h"



Context* nuevoContext(Context* anterior) {
    Context* nuevo = malloc(sizeof(Context));
    nuevo->anterior = anterior;
    if (anterior) {
        nuevo->nombre = anterior->nombre + 1;
    }
    nuevo->ultimoSymbol = NULL;

    return nuevo;
}

Symbol* nuevoVariable(char* nombre, void* valor, TipoDato tipo) {
    Symbol* nuevo = malloc(sizeof(Symbol));
    nuevo->nombre = nombre;
    nuevo->valor = valor;
    nuevo->tipo = tipo;
    nuevo->clase = VARIABLE;
    nuevo->line = 0; // Valor por defecto
    nuevo->column = 0; // Valor por defecto
    return nuevo;
}

// Actualiza la firma y la llamada al reportero
void agregarSymbol(Context* actual, Symbol* symbol, int line, int column) {
    if (buscarSymbol(actual->ultimoSymbol, symbol->nombre)) {
        printf("La variable de nombre '%s' ya existe.\n", symbol->nombre);
        return;
    }

    // Asignamos la línea y columna antes de reportar
    symbol->line = line;
    symbol->column = column;

    add_symbol_to_report(symbol, actual->nombre); // Pasamos el símbolo completo

    symbol->anterior = actual->ultimoSymbol;
    actual->ultimoSymbol = symbol;
}

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

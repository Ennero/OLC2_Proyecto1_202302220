#include "context.h"
#include "symbol_reporter.h"
#include "error_reporter.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Crea un contexto
Context* nuevoContext(Context* anterior, const char* nombre_contexto) {
    Context* nuevo = malloc(sizeof(Context));
    if (!nuevo) return NULL;

    // Inicializar campos
    nuevo->anterior = anterior;
    nuevo->ultimoSymbol = NULL;

    // Crear nombre completo
    if (anterior && anterior->nombre_completo) {
        size_t len_padre = strlen(anterior->nombre_completo);
        size_t len_nuevo = strlen(nombre_contexto);
        nuevo->nombre_completo = malloc(len_padre + len_nuevo + 2); // +2 para '_' y '\0'
        if (nuevo->nombre_completo) {
            sprintf(nuevo->nombre_completo, "%s_%s", anterior->nombre_completo, nombre_contexto);
        }
    } else {

        // Si no hay padre, este es el contexto global
        nuevo->nombre_completo = strdup(nombre_contexto);
    }
    
    return nuevo;
}

void liberarContext(Context* context) {
    if (!context) return;

    Symbol* actual = context->ultimoSymbol;
    while(actual) {
        Symbol* temp = actual->anterior;
        // Aquí se debería liberar la memoria del valor si fue asignado con malloc
        // Por ahora, solo liberamos la estructura del símbolo
        free(actual);
        actual = temp;
    }

    free(context->nombre_completo);
    free(context);
}

// Función para crear una nueva variable
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

// Función para buscar un símbolo en la tabla de símbolos
Symbol* buscarSymbol(Symbol* actual, char* nombre) {
    while (actual) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return actual;
        }
        actual = actual->anterior;
    }
    return NULL;
}

// Función para buscar un símbolo en el contexto actual y en los anteriores
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


// LÓGICA SEMÁNTICA CLAVE
void agregarSymbol(Context* actual, Symbol* symbol, int line, int column) {
    
    // buscarSymbol solo busca en el ámbito (contexto) actual.
    if (buscarSymbol(actual->ultimoSymbol, symbol->nombre)) {
        char description[256];
        snprintf(description, sizeof(description), "La variable '%s' ya ha sido declarada en este ámbito.", symbol->nombre);
        
        // En lugar de printf, se reporta un error semántico formal.
        add_error_to_report("Semántico", symbol->nombre, description, line, column);
        
        // Se libera la memoria del símbolo recién creado que no se usará.
        free(symbol);
        return;
    }

    symbol->line = line;
    symbol->column = column;

    add_symbol_to_report(symbol, actual->nombre_completo);

    symbol->anterior = actual->ultimoSymbol;
    actual->ultimoSymbol = symbol;
}

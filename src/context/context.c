#include "context.h"
#include "symbol_reporter.h"
#include "error_reporter.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array_value.h"

// Crea un contexto
Context* nuevoContext(Context* anterior, const char* nombre_contexto) {
    Context* nuevo = malloc(sizeof(Context));
    if (!nuevo) return NULL;

    // Inicializar campos
    nuevo->anterior = anterior;
    nuevo->ultimoSymbol = NULL;
    
    // Lógica para el puntero raíz y el contador (esto está bien)
    if (anterior) {
        nuevo->raiz = anterior->raiz;
    } else {
        nuevo->raiz = nuevo;
        nuevo->proximo_id_bloque = 1;
    }
    // Crear nombre completo
    if (anterior && anterior->nombre_completo) {
        // Calcular el tamaño necesario para el nuevo nombre
        size_t len_padre = strlen(anterior->nombre_completo);
        size_t len_nuevo = strlen(nombre_contexto);

        // Reservar la memoria
        nuevo->nombre_completo = malloc(len_padre + len_nuevo + 2); // +2 para '_' y '\0'
        
        // Escribir en la memoria reservada
        if (nuevo->nombre_completo) {
            sprintf(nuevo->nombre_completo, "%s_%s", anterior->nombre_completo, nombre_contexto);
        }
    } else {
        // Esto estaba bien: para el contexto global, strdup ya reserva la memoria
        nuevo->nombre_completo = strdup(nombre_contexto);
    }
    
    return nuevo;
}



void liberarContext(Context* context) {
    if (!context) return;

    Symbol* actual = context->ultimoSymbol;
    while(actual) {
        Symbol* temp = actual->anterior;
        
        if (actual->clase == VARIABLE) {

            if (!actual->info.var.borrowed) {
                if (actual->tipo == ARRAY && actual->info.var.valor != NULL) {
                    liberarArray((ArrayValue*)actual->info.var.valor);
                } else if (actual->info.var.valor != NULL) {
                    free(actual->info.var.valor);
                }
            }
        }

        // Si el símbolo es una FUNCIÓN, debemos liberar los nombres de sus parámetros
        if (actual->clase == FUNCION) {
            for (size_t i = 0; i < actual->info.func.num_parametros; i++) {
                free(actual->info.func.parametros[i].nombre);
            }
            free(actual->info.func.parametros);
        }

        // Ahora liberamos el nombre del símbolo y la estructura Symbol en sí
        free(actual->nombre);
        free(actual);
        actual = temp;
    }

    free(context->nombre_completo);
    free(context);
}
// Función para crear una nueva variable
Symbol* nuevoVariable(char* nombre, void* valor, TipoDato tipo, int es_constante) {
    Symbol* nuevo = malloc(sizeof(Symbol));
    if (!nuevo) return NULL;
    // Duplicar el nombre para que el AST y la tabla de símbolos no compartan memoria.
    // Esto evita usar punteros liberados cuando el mismo nodo AST se re-ejecuta (por ejemplo, en for anidados).
    nuevo->nombre = strdup(nombre);
    nuevo->tipo = tipo;
    nuevo->clase = VARIABLE;
    nuevo->info.var.valor = valor;
    nuevo->info.var.borrowed = 0;
    nuevo->es_constante = es_constante;
    nuevo->line = 0;
    nuevo->column = 0;
    nuevo->anterior = NULL;
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


void agregarSymbol(Context* actual, Symbol* symbol, int line, int column) {
    
    // buscarSymbol solo busca en el ámbito (contexto) actual.
    if (buscarSymbol(actual->ultimoSymbol, symbol->nombre)) {
        char description[256];
        snprintf(description, sizeof(description), "La variable '%s' ya ha sido declarada en este ámbito.", symbol->nombre);
        
        // En lugar de printf, se reporta un error semántico formal.
        add_error_to_report("Semántico", symbol->nombre, description, line, column, actual->nombre_completo);
        
        // Se libera la memoria del símbolo recién creado que no se usará.
        if (symbol->clase == VARIABLE && symbol->info.var.valor) {
            free(symbol->info.var.valor);
        }
        free(symbol->nombre);
        free(symbol);
        return;
    }

    symbol->line = line;
    symbol->column = column;

    add_symbol_to_report(symbol, actual->nombre_completo);

    symbol->anterior = actual->ultimoSymbol;
    actual->ultimoSymbol = symbol;
}

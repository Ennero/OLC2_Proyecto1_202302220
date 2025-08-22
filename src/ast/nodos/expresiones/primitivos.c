#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "primitivos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NUEVO: Helper para quitar las comillas de un string
char* quitarComillas(const char* s) {
    int len = strlen(s);
    if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
        char* nuevo = (char*)malloc(len - 1);
        strncpy(nuevo, s + 1, len - 2);
        nuevo[len - 2] = '\0';
        return nuevo;
    }
    return strdup(s);
}

Result interpretPrimitivoExpresion(AbstractExpresion* self, Context* context) {
    PrimitivoExpresion* nodo = (PrimitivoExpresion*) self;
    switch (nodo->tipo) {
        case 'I': {
            // MODIFICADO: Asignar memoria para el resultado
            int* valor = malloc(sizeof(int));
            *valor = atoi(nodo->valor);
            return nuevoValorResultado((void*) valor, nodo->tipo);
        }
        case 'S': {
             // MODIFICADO: Asignar memoria para el string sin comillas
            char* val_str = quitarComillas(nodo->valor);
            return nuevoValorResultado((void*) val_str, nodo->tipo);
        }
        case 'F': {
            // MODIFICADO: Asignar memoria para el resultado
            double* valor = malloc(sizeof(double));
            *valor = atof(nodo->valor);
            return nuevoValorResultado((void*) valor, 'F'); // Usar 'F' para float/double
        }
    }
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoPrimitivoExpresion(char* v, char tipo) {
    PrimitivoExpresion* nodo = malloc(sizeof(PrimitivoExpresion));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretPrimitivoExpresion);
    nodo->valor = v;
    nodo->tipo = tipo;
    return (AbstractExpresion*) nodo;
}
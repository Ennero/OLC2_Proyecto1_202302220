#include "primitivos.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

Result interpretPrimitivoExpresion(AbstractExpresion* self, Context* context) {
    (void)context;
    PrimitivoExpresion* nodo = (PrimitivoExpresion*) self;
    
    switch (nodo->tipo) {
        case INT: {
            int* valor_int = malloc(sizeof(int));
            if (strncmp(nodo->valor, "0x", 2) == 0 || strncmp(nodo->valor, "0X", 2) == 0) {
                *valor_int = (int)strtol(nodo->valor, NULL, 16);
            } else {
                *valor_int = atoi(nodo->valor);
            }
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_int, nodo->tipo);
        }
        case FLOAT: {
            float* valor_float = malloc(sizeof(float));
            *valor_float = atof(nodo->valor);
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_float, nodo->tipo);
        }
        case STRING: {
            size_t len = strlen(nodo->valor);
            if (len >= 2) {
                memmove(nodo->valor, nodo->valor + 1, len - 2);
                nodo->valor[len - 2] = '\0';
            }
            return nuevoValorResultado((void*) nodo->valor, nodo->tipo);
        }
        case CHAR: {
            char* valor_char = malloc(sizeof(char));
            if (strlen(nodo->valor) >= 2) {
                *valor_char = nodo->valor[1];
            } else {
                *valor_char = '\0';
            }
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_char, nodo->tipo);
        }
        case BOOLEAN: {
            int* valor_bool = malloc(sizeof(int));
            *valor_bool = (strcmp(nodo->valor, "true") == 0);
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_bool, nodo->tipo);
        }
        default:
            printf("Tipo de dato primitivo no implementado");
    }
    return nuevoValorResultadoVacio();
}

// <<<<< CORRECCIÓN: Se cambia el tipo del segundo parámetro de 'char' a 'TipoDato' >>>>>
AbstractExpresion* nuevoPrimitivoExpresion(char* v, TipoDato tipo) {
    PrimitivoExpresion* nodo = malloc(sizeof(PrimitivoExpresion));
    if (!nodo) return NULL;
    
    buildAbstractExpresion(&nodo->base, interpretPrimitivoExpresion);

    nodo->valor = v;
    nodo->tipo = tipo;
    return (AbstractExpresion*) nodo;
}

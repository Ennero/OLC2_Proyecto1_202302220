#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "primitivos.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> // Necesario para el tipo 'bool'
#include <string.h>  // Necesario para strlen, strcmp, etc.

Result interpretPrimitivoExpresion(AbstractExpresion* self, Context* context) {
    // Se indica al compilador que el parámetro 'context' no se usa intencionalmente en esta función.
    (void)context;
    PrimitivoExpresion* nodo = (PrimitivoExpresion*) self;
    
    switch (nodo->tipo) {
        case INT: {
            int* valorint = malloc(sizeof(int));
            // CORRECCIÓN: Usar strtol con base 16 para hexadecimales.
            // Si el texto empieza con "0x" o "0X", se interpreta como hexadecimal.
            if (nodo->valor[0] == '0' && (nodo->valor[1] == 'x' || nodo->valor[1] == 'X')) {
                *valorint = (int)strtol(nodo->valor, NULL, 16);
            } else { // De lo contrario, se interpreta como decimal.
                *valorint = atoi(nodo->valor);
            }
            free(nodo->valor); // Se libera la memoria del texto original.
            return nuevoValorResultado((void*) valorint, nodo->tipo);
        }
        case FLOAT: { // Maneja los 'double' de la gramática como 'float' internos.
            float* valorfloat = malloc(sizeof(float));
            *valorfloat = atof(nodo->valor);
            free(nodo->valor);
            return nuevoValorResultado((void*) valorfloat, nodo->tipo);
        }
        case STRING: {
            // Elimina las comillas que rodean al string (ej: "hola" -> hola)
            int len = strlen(nodo->valor);
            if (len >= 2 && nodo->valor[0] == '"' && nodo->valor[len-1] == '"') {
                char* val_str = malloc(len - 1);
                strncpy(val_str, nodo->valor + 1, len - 2);
                val_str[len - 2] = '\0'; // Asegurar el fin de cadena.
                free(nodo->valor);
                return nuevoValorResultado((void*)val_str, nodo->tipo);
            }
            // Si por alguna razón no tiene comillas, lo pasa tal cual.
            return nuevoValorResultado((void*) nodo->valor, nodo->tipo);
        }
        case CHAR: {
            char* valorchar = malloc(sizeof(char));
            // El literal es 'c', se extrae el caracter del medio.
            if (strlen(nodo->valor) >= 3 && nodo->valor[0] == '\'' && nodo->valor[2] == '\'') {
                 *valorchar = nodo->valor[1];
            } else {
                 *valorchar = '\0'; // En caso de error.
            }
            free(nodo->valor);
            return nuevoValorResultado((void*) valorchar, nodo->tipo);
        }
        case BOOLEAN: {
            bool* valorbool = malloc(sizeof(bool));
            // Compara el texto con "true". Si son iguales, el resultado es true (1), si no, es false (0).
            *valorbool = (strcmp(nodo->valor, "true") == 0);
            free(nodo->valor);
            return nuevoValorResultado((void*) valorbool, nodo->tipo);
        }
        default:
            printf("Tipo de dato primitivo no implementado\n");
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
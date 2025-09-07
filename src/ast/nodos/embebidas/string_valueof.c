// en: src/ast/nodos/embebidas/string_valueof.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdio.h>  // Para snprintf
#include <stdlib.h>
#include <string.h>

Result interpretStringValueof(AbstractExpresion* self, Context* context) {
    Result arg = self->hijos[0]->interpret(self->hijos[0], context);
    char buffer[1024]; // Un búfer suficientemente grande

    if (arg.valor == NULL) {
        snprintf(buffer, sizeof(buffer), "null");
    } else {
        switch (arg.tipo) {
            case INT:     snprintf(buffer, sizeof(buffer), "%d", *(int*)arg.valor); break;
            case FLOAT:   snprintf(buffer, sizeof(buffer), "%f", *(float*)arg.valor); break;
            case DOUBLE:  snprintf(buffer, sizeof(buffer), "%lf", *(double*)arg.valor); break;
            case BOOLEAN: snprintf(buffer, sizeof(buffer), "%s", (*(int*)arg.valor) ? "true" : "false"); break;
            case CHAR:    snprintf(buffer, sizeof(buffer), "%c", *(int*)arg.valor); break;
            case STRING:  snprintf(buffer, sizeof(buffer), "%s", (char*)arg.valor); break;
            default:      snprintf(buffer, sizeof(buffer), "Object"); break;
        }
    }

    free(arg.valor); // Liberamos el valor original del argumento
    return nuevoValorResultado(strdup(buffer), STRING);
}

AbstractExpresion* nuevoStringValueofExpresion(AbstractExpresion* expr, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretStringValueof, "StringValueof", line, column);
    agregarHijo(nodo, expr);
    return nodo;
}
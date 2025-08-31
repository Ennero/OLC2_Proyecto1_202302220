#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "output_buffer.h"
#include "print.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> // Para el tipo 'bool'

Result interpretPrintExpresion(AbstractExpresion* self, Context* context) {
    AbstractExpresion* listaExpresiones = self->hijos[0];
    for (size_t i = 0; i < listaExpresiones->numHijos; ++i) {
        Result result = listaExpresiones->hijos[i]->interpret(listaExpresiones->hijos[i], context);

        char buffer[256];

        switch (result.tipo) {
            case STRING:
                sprintf(buffer, "%s\n", (char*) result.valor);
                break;
            case INT:
                sprintf(buffer, "%d\n", *(int*)result.valor);
                break;
            case FLOAT:
                sprintf(buffer, "%f\n", *(float*)result.valor);
                break;
            case CHAR: // NUEVO
                sprintf(buffer, "%c\n", *(char*)result.valor);
                break;
            case BOOLEAN: // NUEVO
                sprintf(buffer, "%s\n", *(bool*)result.valor ? "true" : "false");
                break;
            case NULO:
                sprintf(buffer, "NULL\n");
                break;
            default:
                sprintf(buffer, "Tipo no implementado para print\n");
        }

        append_to_output(buffer);
    }
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoPrintExpresion(AbstractExpresion* listaExpresiones) {
    PrintExpresion* nodo = malloc(sizeof(PrintExpresion));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretPrintExpresion, "Print");
    if (listaExpresiones) agregarHijo((AbstractExpresion*) nodo, listaExpresiones);
    return (AbstractExpresion*) nodo;
}
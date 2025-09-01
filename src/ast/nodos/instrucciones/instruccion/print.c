#include "print.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "output_buffer.h"
#include <stdlib.h>
#include <stdio.h>

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
            case DOUBLE:
                // Un double en java tiene 15 decimales
                sprintf(buffer, "%.15g\n", *(double*)result.valor);
                break;
            case BOOLEAN:
                sprintf(buffer, "%s\n", *(int*)result.valor ? "true" : "false");
                break;
            case CHAR: {
                int char_val = *(int*)result.valor;
                sprintf(buffer, "%c\n", char_val);
                break;
            }
            case NULO:
                sprintf(buffer, "NULL\n");
                break;
            default:
                sprintf(buffer, "Tipo no implementado\n");
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

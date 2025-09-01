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
            case FLOAT: // <<<< NUEVO
                sprintf(buffer, "%f\n", *(float*)result.valor);
                break;
            case DOUBLE:
                // <<<< CAMBIO: Se aumenta la precisión a 15 decimales >>>>
                sprintf(buffer, "%.15g\n", *(double*)result.valor);
                break;
            case BOOLEAN:
                sprintf(buffer, "%s\n", *(int*)result.valor ? "true" : "false");
                break;
            case CHAR: { // <<<< CAMBIO: Manejo de char como int (Unicode)
                int char_val = *(int*)result.valor;
                // Si es un caracter ASCII imprimible, se muestra como tal.
                if (char_val >= 32 && char_val <= 126) {
                    sprintf(buffer, "%c\n", char_val);
                } else { // Si no, se muestra como código Unicode
                    sprintf(buffer, "\\u%04X\n", char_val);
                }
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

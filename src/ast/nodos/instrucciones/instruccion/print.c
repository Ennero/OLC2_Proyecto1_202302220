#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "print.h"
#include <stdio.h>
#include <stdlib.h>

Result interpretPrintExpresion(AbstractExpresion* self, Context* context) {
    AbstractExpresion* listaExpresiones = self->hijos[0];

    for (size_t i = 0; i < listaExpresiones->numHijos; ++i) {
        Result result = listaExpresiones->hijos[i]->interpret(listaExpresiones->hijos[i], context);
        
        char buffer_temp[100]; // Buffer temporal para formatear números

        switch (result.tipo) {
            case 'S':
                agregarAlOutput(context, (char*) result.valor);
                break;
            case 'I':
                sprintf(buffer_temp, "%d", *(int*)result.valor);
                agregarAlOutput(context, buffer_temp);
                break;
            case 'F':
                sprintf(buffer_temp, "%f", *(double*)result.valor);
                agregarAlOutput(context, buffer_temp);
                break;
            default:
                agregarAlOutput(context, "Tipo de dato desconocido");
                break;
        }

        // Liberar la memoria del valor obtenido tras imprimirlo
        if (result.valor != NULL) {
            free(result.valor);
        }

        // Añadir un espacio entre valores, excepto para el último
        if (i < listaExpresiones->numHijos - 1) {
            agregarAlOutput(context, " ");
        }
    }

    agregarAlOutput(context, "\n"); // Salto de línea al final de cada print
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoPrintExpresion(AbstractExpresion* listaExpresiones) {
    PrintExpresion* nodo = malloc(sizeof(PrintExpresion));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretPrintExpresion);
    if (listaExpresiones) agregarHijo((AbstractExpresion*) nodo, listaExpresiones);
    return (AbstractExpresion*) nodo;
}
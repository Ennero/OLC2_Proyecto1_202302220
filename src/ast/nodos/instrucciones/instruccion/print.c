#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "output_buffer.h"
#include "print.h"

#include <stdlib.h>
#include <stdio.h>

// Interpreta una expresión de impresión
Result interpretPrintExpresion(AbstractExpresion* self, Context* context) {
    // recorremos  cada expresion y lo que nos devuelva imprimimos el valor
    AbstractExpresion* listaExpresiones = self->hijos[0];
    for (size_t i = 0; i < listaExpresiones->numHijos; ++i) {
        Result result = listaExpresiones->hijos[i]->interpret(listaExpresiones->hijos[i], context);

        // Formatear el resultado en un buffer
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
            case NULO:
                sprintf(buffer, "NULL\n");
                break;
            default:
                sprintf(buffer, "Tipo no implementado\n");
        }

        // Agregar el buffer al output
        append_to_output(buffer);
    }
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoPrintExpresion(AbstractExpresion* listaExpresiones) {
    //reservar el espacio en memoria y obtener el puntero a este
    PrintExpresion* nodo = malloc(sizeof(PrintExpresion));
    if (!nodo) return NULL;
    //asignar valores
    buildAbstractExpresion(&nodo->base, interpretPrintExpresion);

    if (listaExpresiones) agregarHijo((AbstractExpresion*) nodo, listaExpresiones);
    return (AbstractExpresion*) nodo;
}
#include "print.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "output_buffer.h"
#include <stdlib.h>
#include <stdio.h>

#include "output_buffer.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// Función para convertir un valor a string para imprimir
static void result_to_string(Result res, char *buffer, size_t size)
{
    if (res.valor == NULL)
    {
        snprintf(buffer, size, "null");
        return;
    }
    switch (res.tipo)
    {
    case INT:
        snprintf(buffer, size, "%d", *(int *)res.valor);
        break;
    case FLOAT:
        snprintf(buffer, size, "%f", *(float *)res.valor);
        break;
    case DOUBLE:
        snprintf(buffer, size, "%lf", *(double *)res.valor);
        break;
    case BOOLEAN:
        snprintf(buffer, size, "%s", *(int *)res.valor ? "true" : "false");
        break;
    case CHAR:
        // El valor es un int*, pero lo imprimimos como un carácter.
        snprintf(buffer, size, "%c", *(int *)res.valor);
        break;
    case STRING:
        snprintf(buffer, size, "%s", (char *)res.valor);
        break;
    default:
        snprintf(buffer, size, "Tipo de dato desconocido");
        break;
    }
}

Result interpretPrintExpresion(AbstractExpresion* self, Context* context) {
    AbstractExpresion* listaExpresiones = self->hijos[0];
    for (size_t i = 0; i < listaExpresiones->numHijos; ++i) {
        Result res = listaExpresiones->hijos[i]->interpret(listaExpresiones->hijos[i], context);


        // --- DEBUGGING PRINT ---
        printf("DEBUG [print]: Recibido para imprimir -> Tipo: %s, Puntero a Valor: %p\n", labelTipoDato[res.tipo], res.valor);
        if (res.valor) {
            if (res.tipo == BOOLEAN) printf("DEBUG [print]: Valor Booleano es: %s\n", (*(int*)res.valor) ? "true" : "false");
            if (res.tipo == STRING) printf("DEBUG [print]: Valor String es: '%s'\n", (char*)res.valor);
        }


        if (has_semantic_error_been_found()) {
            if(res.valor) free(res.valor);
            return nuevoValorResultadoVacio();
        }

        char print_buffer[1024];
        result_to_string(res, print_buffer, sizeof(print_buffer));
        append_to_output(print_buffer);
        
        if (i < listaExpresiones->numHijos - 1) append_to_output(" ");

        if(res.valor) free(res.valor);
    }
    append_to_output("\n");
    return nuevoValorResultadoVacio();
}

AbstractExpresion *nuevoPrintExpresion(AbstractExpresion *listaExpresiones, int line, int column)
{
    PrintExpresion *nodo = malloc(sizeof(PrintExpresion));
    if (!nodo)
        return NULL;
    buildAbstractExpresion(&nodo->base, interpretPrintExpresion, "Print", line, column);

    if (listaExpresiones)
        agregarHijo((AbstractExpresion *)nodo, listaExpresiones);
    return (AbstractExpresion *)nodo;
}

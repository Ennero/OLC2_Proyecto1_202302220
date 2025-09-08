// en: src/ast/nodos/propiedades/array_length.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>

Result interpretArrayLength(AbstractExpresion* self, Context* context) {
    // El hijo 0 es la expresión que debería resultar en un arreglo
    Result arr_res = self->hijos[0]->interpret(self->hijos[0], context);

    if (arr_res.tipo != ARRAY) {
        add_error_to_report("Semantico", ".length", "La propiedad 'length' solo se puede usar en arreglos.", self->line, self->column, context->nombre_completo);
        free(arr_res.valor);
        return nuevoValorResultadoVacio();
    }
    
    ArrayValue* arr = (ArrayValue*)arr_res.valor;
    int* tamano = malloc(sizeof(int));
    *tamano = arr->tamano;

    // No liberar el arreglo aquí: normalmente es una referencia a una variable existente
    // y no somos dueños de su memoria en esta operación.

    return nuevoValorResultado(tamano, INT);
}

AbstractExpresion* nuevoArrayLengthExpresion(AbstractExpresion* arr_expr, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretArrayLength, "ArrayLength", line, column);
    agregarHijo(nodo, arr_expr);
    return nodo;
}
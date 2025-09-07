// en: src/ast/nodos/embebidas/arrays_indexof.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <string.h>

// Función auxiliar para comparar dos valores de tipo Result
static int sonValoresIguales(Result r1, Result r2) {
    if (r1.tipo != r2.tipo) return 0;
    if (r1.valor == NULL || r2.valor == NULL) return r1.valor == r2.valor;

    switch (r1.tipo) {
        case INT: case BOOLEAN: case CHAR:
            return *(int*)r1.valor == *(int*)r2.valor;
        case FLOAT:
            return *(float*)r1.valor == *(float*)r2.valor;
        case DOUBLE:
            return *(double*)r1.valor == *(double*)r2.valor;
        case STRING:
            return strcmp((char*)r1.valor, (char*)r2.valor) == 0;
        default:
            return 0;
    }
}

Result interpretArraysIndexof(AbstractExpresion* self, Context* context) {
    Result arr_res = self->hijos[0]->interpret(self->hijos[0], context);
    Result val_res = self->hijos[1]->interpret(self->hijos[1], context);

    if (arr_res.tipo != ARRAY) {
        add_error_to_report("Semantico", "Arrays.indexOf", "El primer argumento debe ser un arreglo.", self->line, self->column, context->nombre_completo);
        free(arr_res.valor); free(val_res.valor);
        return nuevoValorResultadoVacio();
    }
    
    ArrayValue* arr = (ArrayValue*)arr_res.valor;
    int* indice_encontrado = malloc(sizeof(int));
    *indice_encontrado = -1; // Valor por defecto si no se encuentra

    for (int i = 0; i < arr->tamano; i++) {
        if (sonValoresIguales(arr->valores[i], val_res)) {
            *indice_encontrado = i;
            break; // Terminar la búsqueda al encontrar la primera coincidencia
        }
    }

    liberarArray(arr);
    free(val_res.valor);

    return nuevoValorResultado(indice_encontrado, INT);
}

AbstractExpresion* nuevoArraysIndexofExpresion(AbstractExpresion* arr_expr, AbstractExpresion* val_expr, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretArraysIndexof, "ArraysIndexof", line, column);
    agregarHijo(nodo, arr_expr);
    agregarHijo(nodo, val_expr);
    return nodo;
}
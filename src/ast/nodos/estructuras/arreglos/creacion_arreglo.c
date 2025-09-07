// en: src/ast/nodos/estructuras/arreglos/creacion_arreglo.c
#include "ast/nodos/builders.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>

static ArrayValue* crearDimension(AbstractExpresion* self, Context* context, TipoDato tipo_base, AbstractExpresion* lista_dims, size_t dim_actual) {
    if (dim_actual >= (size_t)lista_dims->numHijos) {
        return NULL; // Caso base
    }

    Result res_tam = lista_dims->hijos[dim_actual]->interpret(lista_dims->hijos[dim_actual], context);
    if (res_tam.tipo != INT) {
        add_error_to_report("Semantico", "new", "El tamaño de un arreglo debe ser un entero.", self->line, self->column, context->nombre_completo);
        free(res_tam.valor);
        return NULL;
    }

    int tamano = *(int*)res_tam.valor;
    free(res_tam.valor);

    ArrayValue* arr = nuevoArray(tipo_base, (int)(lista_dims->numHijos - (int)dim_actual), tamano);

    // Si no es la última dimensión, crear los sub-arreglos
    if (((int)lista_dims->numHijos - (int)dim_actual) > 1) {
        for (int i = 0; i < tamano; i++) {
            arr->valores[i].tipo = ARRAY;
            arr->valores[i].valor = crearDimension(self, context, tipo_base, lista_dims, dim_actual + 1U);
        }
    }
    return arr;
}

Result interpretCreacionArreglo(AbstractExpresion* self, Context* context) {
    Result tipo_res = self->hijos[0]->interpret(self->hijos[0], context);
    TipoDato tipo_base = tipo_res.tipo;
    AbstractExpresion* lista_dims = self->hijos[1];

    ArrayValue* array_final = crearDimension(self, context, tipo_base, lista_dims, 0U);
    return nuevoValorResultado(array_final, ARRAY);
}

AbstractExpresion* nuevoCreacionArreglo(AbstractExpresion* tipo, AbstractExpresion* dimensiones, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretCreacionArreglo, "ArrayCreation", line, column);
    agregarHijo(nodo, tipo);
    agregarHijo(nodo, dimensiones);
    return nodo;
}
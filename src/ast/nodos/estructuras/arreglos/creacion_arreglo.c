// en: src/ast/nodos/estructuras/arreglos/creacion_arreglo.c
#include "ast/nodos/builders.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>

// Helper iterativo/recursivo que respeta dims vacías (arreglos irregulares)
static ArrayValue* crearDimensionJagged(
    AbstractExpresion* self,
    Context* context,
    TipoDato tipo_base,
    AbstractExpresion* lista_dims,
    size_t dim_actual,
    int trailing_empty
) {
    size_t expr_dims = (size_t)lista_dims->numHijos;
    if (dim_actual >= expr_dims) {
        // No debería pedirse crear más allá de las expr dims aquí
        return NULL;
    }

    Result res_tam = lista_dims->hijos[dim_actual]->interpret(lista_dims->hijos[dim_actual], context);
    if (res_tam.tipo != INT) {
        add_error_to_report("Semantico", "new", "El tamaño de un arreglo debe ser un entero.", self->line, self->column, context->nombre_completo);
        free(res_tam.valor);
        return NULL;
    }
    int tamano = *(int*)res_tam.valor;
    free(res_tam.valor);

    int remaining_expr = (int)(expr_dims - dim_actual);
    int remaining_total = remaining_expr + trailing_empty;
    ArrayValue* arr = nuevoArray(tipo_base, remaining_total, tamano);

    // Si quedan más dimensiones totales
    if (remaining_total > 1) {
        for (int i = 0; i < tamano; ++i) {
            arr->valores[i].tipo = ARRAY;
            if (remaining_expr > 1) {
                // Aún hay expr dims por especificar: crear sub-arreglo recursivamente
                arr->valores[i].valor = crearDimensionJagged(self, context, tipo_base, lista_dims, dim_actual + 1U, trailing_empty);
            } else {
                // Ya no hay expr dims, solo quedan dims vacías: dejar NULL (jagged)
                arr->valores[i].valor = NULL;
            }
        }
    }
    // Si remaining_total == 1, nuevoArray ya llenó valores por defecto del tipo base.
    return arr;
}

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

    // Determinar si hay dims vacías adicionales
    int trailing_empty = 0;
    if (self->numHijos > 2) {
        Result r = self->hijos[2]->interpret(self->hijos[2], context);
        if (r.tipo == INT && r.valor) {
            trailing_empty = *(int*)r.valor;
        }
        if (r.valor) free(r.valor);
    }

    ArrayValue* array_final;
    if (trailing_empty == 0) {
        // Comportamiento previo: dimensiones totalmente especificadas
        array_final = crearDimension(self, context, tipo_base, lista_dims, 0U);
    } else {
        // Crear solo las expr dims y dejar las restantes vacías (NULL)
        array_final = crearDimensionJagged(self, context, tipo_base, lista_dims, 0U, trailing_empty);
    }
    return nuevoValorResultado(array_final, ARRAY);
}

AbstractExpresion* nuevoCreacionArreglo(AbstractExpresion* tipo, AbstractExpresion* dimensiones, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretCreacionArreglo, "ArrayCreation", line, column);
    agregarHijo(nodo, tipo);
    agregarHijo(nodo, dimensiones);
    return nodo;
}
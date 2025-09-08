// en: src/ast/nodos/instrucciones/ciclo/for_each.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    AbstractExpresion base;
    TipoDato tipo_var;
    int dimensiones_var; // Para soportar 'for (int[] fila : matriz)'
    char* nombre_var;
    int line;
    int column;
} ForEachExpresion;

Result interpretForEach(AbstractExpresion* self, Context* context) {
    ForEachExpresion* nodo = (ForEachExpresion*)self;
    Result arr_res = self->hijos[0]->interpret(self->hijos[0], context);

    if (arr_res.tipo != ARRAY) {
        add_error_to_report("Semantico", "for", "La expresión en un for-each debe ser un arreglo.", nodo->line, nodo->column, context->nombre_completo);
        free(arr_res.valor);
        return nuevoValorResultadoVacio();
    }
    
    ArrayValue* arr = (ArrayValue*)arr_res.valor;
        TipoDato tipo_variable_en_scope = (nodo->dimensiones_var > 0) ? ARRAY : nodo->tipo_var;

    // --- VALIDACIÓN DE TIPOS MEJORADA ---
    // Comprueba que las dimensiones de la variable del bucle sean una menos que las del arreglo.
    if (nodo->tipo_var != arr->tipo_elemento_base || nodo->dimensiones_var != arr->dimensiones_total - 1) {
        add_error_to_report("Semantico", "for", "El tipo de la variable del bucle es incompatible con los elementos del arreglo.", nodo->line, nodo->column, context->nombre_completo);
        // No liberar 'arr' aquí: no somos dueños del arreglo iterado.
        return nuevoValorResultadoVacio();
    }

    Context* for_context = nuevoContext(context, "forEach_loop");
    Result result_bloque = nuevoValorResultadoVacio();

    for (int i = 0; i < arr->tamano; i++) {
        Symbol* var_iteracion = buscarSymbol(for_context->ultimoSymbol, nodo->nombre_var);
        
        // Hacemos una copia profunda del elemento para no modificar el arreglo original
        void* valor_copiado = NULL;
        if (arr->valores[i].tipo == ARRAY) {
            valor_copiado = copiarArray((ArrayValue*)arr->valores[i].valor);
        } else {
            // Lógica de copia de primitivos (extraída de tu función 'copiarValor')
            Result elem = arr->valores[i];
            if (elem.valor != NULL) {
                if (elem.tipo == STRING) {
                    valor_copiado = strdup((char*)elem.valor);
                } else {
                    size_t size = sizeof(int);
                    if (elem.tipo == FLOAT) size = sizeof(float);
                    if (elem.tipo == DOUBLE) size = sizeof(double);
                    valor_copiado = malloc(size);
                    memcpy(valor_copiado, elem.valor, size);
                }
            }
        }
        
        if (var_iteracion) {
            if (var_iteracion->tipo == ARRAY) {
                if (var_iteracion->info.var.valor) {
                    liberarArray((ArrayValue*)var_iteracion->info.var.valor);
                }
            } else {
                free(var_iteracion->info.var.valor);
            }
            var_iteracion->info.var.valor = valor_copiado;
        } else {
            agregarSymbol(for_context, nuevoVariable(nodo->nombre_var, valor_copiado, tipo_variable_en_scope, 0), nodo->line, nodo->column);
        }

        result_bloque = self->hijos[1]->interpret(self->hijos[1], for_context);

        // Manejo de continue: limpiar y seguir con la siguiente iteración
        if (result_bloque.tipo == CONTINUE_T) {
            free(result_bloque.valor);           // seguro aunque sea NULL
            result_bloque = nuevoValorResultadoVacio();
            continue;
        }
        // Manejo de break y return: detener el bucle. Solo RETURN se propagará al exterior
        if (result_bloque.tipo == BREAK_T || result_bloque.tipo == RETURN_T) {
            break;
        }

        // En iteraciones normales, liberar el posible valor devuelto por el bloque
        free(result_bloque.valor);
        result_bloque = nuevoValorResultadoVacio();
    }

    // No liberar 'arr' aquí: suele ser referencia a una variable del contexto
    liberarContext(for_context);

    // Solo un RETURN debe salir del for-each hacia niveles superiores (función, etc.)
    if (result_bloque.tipo == RETURN_T) {
        return result_bloque;
    }
    // Un BREAK solo detiene el bucle; no debe propagarse fuera. Limpiamos cualquier valor.
    free(result_bloque.valor); // safe on NULL
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoForEachExpresion(TipoDato tipo, int dimensiones, char* nombre, AbstractExpresion* array_expr, AbstractExpresion* bloque, int line, int column) {
    ForEachExpresion* nodo = malloc(sizeof(ForEachExpresion));
    buildAbstractExpresion(&nodo->base, interpretForEach, "ForEach", line, column);
    nodo->tipo_var = tipo;
    nodo->dimensiones_var = dimensiones;
    nodo->nombre_var = nombre;
    nodo->line = line;
    nodo->column = column;
    agregarHijo(&nodo->base, array_expr);
    agregarHijo(&nodo->base, bloque);
    return &nodo->base;
}
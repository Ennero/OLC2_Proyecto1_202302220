// en: src/ast/nodos/estructuras/arreglos/asignacion_arreglo.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <string.h>

// Prototipo para acceder a la estructura del nodo Identificador
#include "ast/nodos/expresiones/terminales/identificadores.h"

Result interpretAsignacionArreglo(AbstractExpresion *self, Context *context)
{
    AbstractExpresion* acceso_nodo = self->hijos[0];
    AbstractExpresion* valor_nodo = self->hijos[1];

    // Paso 1: Interpretar la expresión base del acceso (ej. 'a' en a[i])
    Result res_arr_base = acceso_nodo->hijos[0]->interpret(acceso_nodo->hijos[0], context);
    Result res_idx = acceso_nodo->hijos[1]->interpret(acceso_nodo->hijos[1], context);

    // --- MANEJO DE ERRORES ---
    if (res_arr_base.tipo != ARRAY)
    {
        add_error_to_report("Semantico", "[]", "Se intentó asignar a un elemento de una variable que no es un arreglo.", self->line, self->column, context->nombre_completo);
        free(res_arr_base.valor);
        free(res_idx.valor);
        return nuevoValorResultadoVacio();
    }
    if (res_idx.tipo != INT)
    {
        add_error_to_report("Semantico", "[]", "El índice de un arreglo debe ser un entero.", self->line, self->column, context->nombre_completo);
        free(res_arr_base.valor);
        free(res_idx.valor);
        return nuevoValorResultadoVacio();
    }
    // --- FIN MANEJO DE ERRORES ---

    ArrayValue* arr_a_modificar = (ArrayValue*)res_arr_base.valor;
    int index = *(int*)res_idx.valor;
    free(res_idx.valor);

    // --- MANEJO DE ERRORES ---
    if (index < 0 || index >= arr_a_modificar->tamano)
    {
        add_error_to_report("Semantico", "[]", "Índice fuera de los límites del arreglo en una asignación.", self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }
    // --- FIN MANEJO DE ERRORES ---

    Result nuevo_valor = valor_nodo->interpret(valor_nodo, context);

    // --- MANEJO DE ERRORES ---
    if (arr_a_modificar->tipo_elemento_base != nuevo_valor.tipo)
    {
        add_error_to_report("Semantico", "=", "Tipo incompatible en la asignación al elemento del arreglo.", self->line, self->column, context->nombre_completo);
        free(nuevo_valor.valor);
        return nuevoValorResultadoVacio();
    }
    // --- FIN MANEJO DE ERRORES ---

    // Liberamos el valor antiguo en esa posición y lo reemplazamos
    free(arr_a_modificar->valores[index].valor);
    arr_a_modificar->valores[index] = nuevo_valor;

    // No liberar el arreglo base: es propiedad de la tabla de símbolos
    
    return nuevoValorResultadoVacio();
}

// El constructor no necesita cambios
AbstractExpresion* nuevoAsignacionArreglo(AbstractExpresion* base, AbstractExpresion* indice, AbstractExpresion* expr, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretAsignacionArreglo, "ArrayAssignment", line, column);
    
    // Creamos el nodo de acceso que representa el lado izquierdo
    AbstractExpresion* acceso = nuevoAccesoArreglo(base, indice, line, column);
    
    // El nodo de asignación tiene 2 hijos: el acceso y la expresión a asignar
    agregarHijo(nodo, acceso);
    agregarHijo(nodo, expr);
    
    return nodo;
}
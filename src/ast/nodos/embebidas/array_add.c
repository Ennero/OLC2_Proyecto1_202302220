// en: src/ast/nodos/embebidas/array_add.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h" // Necesario para la estructura y funciones de arreglos
#include "error_reporter.h"
#include <stdlib.h>
#include <string.h>

Result interpretArrayAdd(AbstractExpresion *self, Context *context)
{
    Result arr_res = self->hijos[0]->interpret(self->hijos[0], context);
    Result elem_res = self->hijos[1]->interpret(self->hijos[1], context);

    if (arr_res.tipo != ARRAY)
    {
        add_error_to_report("Semantico", "Array.add", "El primer argumento debe ser un arreglo.", self->line, self->column, context->nombre_completo);
        free(arr_res.valor);
        free(elem_res.valor);
        return nuevoValorResultadoVacio();
    }

    ArrayValue *arr_original = (ArrayValue *)arr_res.valor;

    // --- INICIO DE LA LÓGICA DE VALIDACIÓN CORREGIDA ---
    int tipos_compatibles = 0; // Usaremos esto como un booleano

    // CASO 1: El elemento a añadir es un arreglo (ej. una fila a una matriz)
    if (elem_res.tipo == ARRAY)
    {
        ArrayValue *elem_arr = (ArrayValue *)elem_res.valor;
        // Los tipos base deben ser iguales Y el contenedor debe tener una dimensión más.
        if (arr_original->tipo_elemento_base == elem_arr->tipo_elemento_base &&
            arr_original->dimensiones_total == elem_arr->dimensiones_total + 1)
        {
            tipos_compatibles = 1;
        }
    }
    // CASO 2: El elemento a añadir es un primitivo
    else
    {
        // Solo es válido si el contenedor es un arreglo de 1D y los tipos base coinciden.
        if (arr_original->dimensiones_total == 1 &&
            arr_original->tipo_elemento_base == elem_res.tipo)
        {
            tipos_compatibles = 1;
        }
    }

    if (!tipos_compatibles)
    {
        add_error_to_report("Semantico", "Array.add", "El tipo del nuevo elemento es incompatible con el tipo del arreglo.", self->line, self->column, context->nombre_completo);
        // No liberar arr_original: no somos dueños (normalmente es referencia desde la tabla de símbolos)
        if (elem_res.tipo != ARRAY) free(elem_res.valor); // liberar solo temporales primitivos
        return nuevoValorResultadoVacio();
    }
    // --- FIN DE LA LÓGICA DE VALIDACIÓN CORREGIDA ---

    int nuevo_tamano = arr_original->tamano + 1;
    ArrayValue *arr_nuevo = nuevoArray(arr_original->tipo_elemento_base, arr_original->dimensiones_total, nuevo_tamano);

    // Copiamos los elementos antiguos. Ahora la copia debe ser profunda.
    for (int i = 0; i < arr_original->tamano; i++)
    {
        free(arr_nuevo->valores[i].valor); // Liberar el valor por defecto

        // Hacemos una copia profunda del elemento para evitar compartir punteros.
        if (arr_original->valores[i].tipo == ARRAY)
        {
            arr_nuevo->valores[i] = nuevoValorResultado(copiarArray(arr_original->valores[i].valor), ARRAY);
        }
        else
        {
            // Copia de primitivos (código que ya tenías en copiarArray)
            arr_nuevo->valores[i].tipo = arr_original->valores[i].tipo;
            if (arr_original->valores[i].tipo == STRING)
            {
                arr_nuevo->valores[i].valor = strdup((char *)arr_original->valores[i].valor);
            }
            else
            {
                size_t size = sizeof(int); // Para INT, CHAR, BOOLEAN
                if (arr_original->valores[i].tipo == FLOAT)
                    size = sizeof(float);
                if (arr_original->valores[i].tipo == DOUBLE)
                    size = sizeof(double);
                arr_nuevo->valores[i].valor = malloc(size);
                memcpy(arr_nuevo->valores[i].valor, arr_original->valores[i].valor, size);
            }
        }
    }

    // Añadir el nuevo elemento al final
    free(arr_nuevo->valores[nuevo_tamano - 1].valor); // liberar valor por defecto
    if (elem_res.tipo == ARRAY) {
        // Copia profunda para evitar aliasing con variables existentes (p. ej., agregar una fila existente a una matriz)
        ArrayValue* copia = copiarArray((ArrayValue*)elem_res.valor);
        arr_nuevo->valores[nuevo_tamano - 1] = nuevoValorResultado(copia, ARRAY);
        // No liberar elem_res.valor (referencia no-propietaria)
    } else {
        // Transferir propiedad de primitivos directamente
        arr_nuevo->valores[nuevo_tamano - 1] = elem_res;
    }

    // No liberar el arreglo original aquí; normalmente es una referencia administrada por el contexto.
    return nuevoValorResultado(arr_nuevo, ARRAY);
}

AbstractExpresion *nuevoArrayAddExpresion(AbstractExpresion *arr_expr, AbstractExpresion *elem_expr, int line, int column)
{
    AbstractExpresion *nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretArrayAdd, "ArrayAdd", line, column);
    agregarHijo(nodo, arr_expr);
    agregarHijo(nodo, elem_expr);
    return nodo;
}
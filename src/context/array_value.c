// en: src/context/array_value.c
#include "array_value.h"
#include <stdlib.h>
#include <string.h>

Result getDefaultValueForType(TipoDato tipo) {
    void* default_value = NULL;
    switch (tipo) {
        case INT: case CHAR: case BOOLEAN:
            default_value = malloc(sizeof(int));
            *(int*)default_value = 0;
            break;
        case FLOAT:
            default_value = malloc(sizeof(float));
            *(float*)default_value = 0.0f;
            break;
        case DOUBLE:
            default_value = malloc(sizeof(double));
            *(double*)default_value = 0.0;
            break;
        case STRING: default_value = NULL; break;
        default: break;
    }
    return nuevoValorResultado(default_value, tipo);
}

ArrayValue* nuevoArray(TipoDato tipo_base, int dimensiones, int tamano) {
    ArrayValue* arr = malloc(sizeof(ArrayValue));
    arr->tipo_elemento_base = tipo_base;
    arr->dimensiones_total = dimensiones;
    arr->tamano = tamano;
    arr->valores = calloc(tamano, sizeof(Result)); // calloc inicializa a 0/NULL

    // Si es la última dimensión, inicializamos con valores por defecto del tipo base.
    if (dimensiones == 1) {
        for (int i = 0; i < tamano; i++) {
            arr->valores[i] = getDefaultValueForType(tipo_base);
        }
    }
    return arr;
}

void liberarArray(ArrayValue* arr) {
    if (!arr) return;
    for (int i = 0; i < arr->tamano; i++) {
        // Si los elementos son sub-arreglos, liberarlos recursivamente.
        if (arr->valores[i].tipo == ARRAY) {
            liberarArray((ArrayValue*)arr->valores[i].valor);
        } else {
            // Si no, liberar el valor primitivo.
            free(arr->valores[i].valor);
        }
    }
    free(arr->valores);
    free(arr);
}

// Función auxiliar para obtener el tamaño de un tipo (evita duplicar código)
static size_t get_type_size_internal(TipoDato tipo) {
    switch (tipo) {
        case INT: case BOOLEAN: case CHAR: return sizeof(int);
        case FLOAT: return sizeof(float);
        case DOUBLE: return sizeof(double);
        default: return 0;
    }
}

// Realiza una copia profunda (recursiva) de un ArrayValue
ArrayValue* copiarArray(ArrayValue* original) {
    if (!original) return NULL;

    // 1. Crear la estructura del nuevo arreglo
    ArrayValue* copia = nuevoArray(original->tipo_elemento_base, original->dimensiones_total, original->tamano);
    
    // 2. Copiar cada elemento
    for (int i = 0; i < original->tamano; i++) {
        Result elem_original = original->valores[i];
        
        // Liberamos el valor por defecto que 'nuevoArray' puso en la copia
        free(copia->valores[i].valor);
        
        copia->valores[i].tipo = elem_original.tipo;

        if (elem_original.tipo == ARRAY) {
            // Si el elemento es otro arreglo, lo copiamos recursivamente
            copia->valores[i].valor = copiarArray((ArrayValue*)elem_original.valor);
        } else {
            // Si es un valor primitivo, hacemos una copia profunda de su valor
            if (elem_original.valor == NULL) {
                copia->valores[i].valor = NULL;
            } else if (elem_original.tipo == STRING) {
                copia->valores[i].valor = strdup((char*)elem_original.valor);
            } else {
                size_t size = get_type_size_internal(elem_original.tipo);
                copia->valores[i].valor = malloc(size);
                memcpy(copia->valores[i].valor, elem_original.valor, size);
            }
        }
    }
    return copia;
}
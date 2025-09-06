// en: src/context/array_value.h
#ifndef ARRAY_VALUE_H
#define ARRAY_VALUE_H

#include "result.h"

// Representa una instancia de un arreglo en memoria.
typedef struct ArrayValue {
    TipoDato tipo_elemento_base; // El tipo primitivo final (INT, STRING, etc.)
    int dimensiones_total;      // Número total de dimensiones. ej. para int[][][] es 3
    
    // Info de esta dimensión específica
    int tamano;                 // El tamaño de esta dimensión del arreglo.
    Result* valores;            // Punteros a los 'Result' de los elementos.
                                // Si es un arreglo multidimensional, el Result.tipo será ARRAY
                                // y Result.valor será otro struct ArrayValue.
} ArrayValue;

// Prototipos de funciones auxiliares
ArrayValue* nuevoArray(TipoDato tipo_base, int dimensiones, int tamano);
void liberarArray(ArrayValue* arr);
Result getDefaultValueForType(TipoDato tipo);

#endif // ARRAY_VALUE_H
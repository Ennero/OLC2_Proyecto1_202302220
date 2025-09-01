#include "aritmeticas.h"
#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h" // Se incluye para UnaryOperacion
#include "context/result.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// --- Funciones de Negación ---
// Reciben directamente un 'Result'
Result negarInt(Result res) {
    int* val = malloc(sizeof(int));
    *val = -(*((int*)res.valor));
    free(res.valor);
    return nuevoValorResultado(val, INT);
}

Result negarFloat(Result res) {
    float* val = malloc(sizeof(float));
    *val = -(*((float*)res.valor));
    free(res.valor);
    return nuevoValorResultado(val, FLOAT);
}

Result negarDouble(Result res) {
    double* val = malloc(sizeof(double));
    *val = -(*((double*)res.valor));
    free(res.valor);
    return nuevoValorResultado(val, DOUBLE);
}

// --- Tabla de Operaciones para Unario (1D) ---
// Ahora usa el tipo correcto 'UnaryOperacion' y es un arreglo simple.
UnaryOperacion tablaOperacionesUnario[TIPO_COUNT] = {
    [INT] = negarInt,
    [FLOAT] = negarFloat,
    [DOUBLE] = negarDouble
};

// --- Interpretador para el Nodo Unario ---
// Esta función es específica para el nodo unario y es más simple.
Result interpretUnarioExpresion(AbstractExpresion* self, Context* context) {
    // 1. Interpretar la expresión hija (solo hay una)
    Result res = self->hijos[0]->interpret(self->hijos[0], context);

    // 2. Comprobar si el tipo es válido para la negación
    if (res.tipo >= TIPO_COUNT || tablaOperacionesUnario[res.tipo] == NULL) {
        char desc[256];
        sprintf(desc, "El operador unario '-' no se puede aplicar a un valor de tipo '%s'.", labelTipoDato[res.tipo]);
        // Idealmente, se pasaría la línea/columna del operador '-'
        add_error_to_report("Semantico", "-", desc, self->line, self->column, context->nombre_completo); 
        return nuevoValorResultadoVacio();
    }
    
    // 3. Llamar a la función de negación correcta
    return tablaOperacionesUnario[res.tipo](res);
}

// --- Constructor del Nodo ---
AbstractExpresion* nuevoUnarioExpresion(AbstractExpresion* expresion, int line, int column) {
    // Nota: El nodo unario no necesita la estructura compleja 'ExpresionLenguaje'.
    // Es un nodo simple con un solo hijo.
    AbstractExpresion* unarioExpresion = malloc(sizeof(AbstractExpresion));
    if(!unarioExpresion) return NULL;

    buildAbstractExpresion(unarioExpresion, interpretUnarioExpresion, "NegacionUnaria", line, column);
    agregarHijo(unarioExpresion, expresion);

    return unarioExpresion;
}
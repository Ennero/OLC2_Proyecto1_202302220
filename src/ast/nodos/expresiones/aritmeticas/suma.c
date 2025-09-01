#include "aritmeticas.h"
#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "context/result.h"
#include <stdlib.h>

// --- Implementaciones ---
Result sumarIntInt(ExpresionLenguaje* self) {
    int* res = malloc(sizeof(int));
    *res =  *((int*)self->izquierda.valor) + *((int*)self->derecha.valor);
    return nuevoValorResultado(res, INT);
}

Result sumarFloatFloat(ExpresionLenguaje* self) {
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) + *((float*)self->derecha.valor);
    return nuevoValorResultado(res, FLOAT);
}

Result sumarDoubleDouble(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) + *((double*)self->derecha.valor);
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarIntFloat(ExpresionLenguaje* self) {
    float* res = malloc(sizeof(float));
    *res = (float)(*((int*)self->izquierda.valor)) + *((float*)self->derecha.valor);
    return nuevoValorResultado(res, FLOAT);
}

Result sumarFloatInt(ExpresionLenguaje* self) {
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) + (float)(*((int*)self->derecha.valor));
    return nuevoValorResultado(res, FLOAT);
}

Result sumarIntDouble(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = (double)(*((int*)self->izquierda.valor)) + *((double*)self->derecha.valor);
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarDoubleInt(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) + (double)(*((int*)self->derecha.valor));
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarFloatDouble(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = (double)(*((float*)self->izquierda.valor)) + *((double*)self->derecha.valor);
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarDoubleFloat(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) + (double)(*((float*)self->derecha.valor));
    return nuevoValorResultado(res, DOUBLE);
}

Operacion tablaOperacionesSuma[TIPO_COUNT][TIPO_COUNT] = {
    [INT][INT] = sumarIntInt,
    [FLOAT][FLOAT] = sumarFloatFloat,
    [DOUBLE][DOUBLE] = sumarDoubleDouble,
    [INT][FLOAT] = sumarIntFloat,
    [FLOAT][INT] = sumarFloatInt,
    [INT][DOUBLE] = sumarIntDouble,
    [DOUBLE][INT] = sumarDoubleInt,
    [FLOAT][DOUBLE] = sumarFloatDouble,
    [DOUBLE][FLOAT] = sumarDoubleFloat
};

AbstractExpresion* nuevoSumaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column) {
    ExpresionLenguaje* sumaExpresion = nuevoExpresionLenguaje(interpretExpresionLenguaje, izquierda, derecha, line, column);
    sumaExpresion->base.node_type = "Suma";
    sumaExpresion->tablaOperaciones = &tablaOperacionesSuma;
    return (AbstractExpresion*) sumaExpresion;
}
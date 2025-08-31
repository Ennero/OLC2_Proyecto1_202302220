#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "aritmeticas.h"
#include <stdlib.h>
#include <stdio.h>

Result dividirIntInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) {
        printf("Error: División por cero.\n");
        return nuevoValorResultadoVacio();
    }
    int* res = malloc(sizeof(int));
    *res = *((int*)self->izquierda.valor) / divisor;
    return nuevoValorResultado(res, INT);
}

Result dividirFloatFloat(ExpresionLenguaje* self) {
    float divisor = *((float*)self->derecha.valor);
    if (divisor == 0.0) {
        printf("Error: División por cero.\n");
        return nuevoValorResultadoVacio();
    }
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) / divisor;
    return nuevoValorResultado(res, FLOAT);
}

Operacion tablaOperacionesDivision[TIPO_COUNT][TIPO_COUNT] = {
    [INT][INT] = dividirIntInt,
    [FLOAT][FLOAT] = dividirFloatFloat,
};

AbstractExpresion* nuevoDivisionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, izquierda, derecha);
    expr->tablaOperaciones = &tablaOperacionesDivision;
    return (AbstractExpresion*) expr;
}
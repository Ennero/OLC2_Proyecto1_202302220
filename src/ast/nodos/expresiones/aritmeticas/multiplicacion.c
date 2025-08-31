#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "aritmeticas.h"
#include <stdlib.h>

Result multiplicarIntInt(ExpresionLenguaje* self) {
    int* res = malloc(sizeof(int));
    *res = *((int*)self->izquierda.valor) * *((int*)self->derecha.valor);
    return nuevoValorResultado(res, INT);
}

Result multiplicarFloatFloat(ExpresionLenguaje* self) {
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) * *((float*)self->derecha.valor);
    return nuevoValorResultado(res, FLOAT);
}

// ... (puedes añadir más combinaciones como int * float)

Operacion tablaOperacionesMultiplicacion[TIPO_COUNT][TIPO_COUNT] = {
    [INT][INT] = multiplicarIntInt,
    [FLOAT][FLOAT] = multiplicarFloatFloat,
};

AbstractExpresion* nuevoMultiplicacionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, izquierda, derecha);
    expr->tablaOperaciones = &tablaOperacionesMultiplicacion;
    return (AbstractExpresion*) expr;
}

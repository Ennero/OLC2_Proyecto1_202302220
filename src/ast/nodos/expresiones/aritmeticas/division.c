#include "aritmeticas.h"
#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "context/result.h"
#include "error_reporter.h"
#include <stdlib.h>

Result dividirIntInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    // La división de enteros en C trunca el resultado, lo cual es correcto para int/int.
    int* res = malloc(sizeof(int));
    *res = *((int*)self->izquierda.valor) / divisor;
    return nuevoValorResultado(res, INT);
}

Result dividirFloatFloat(ExpresionLenguaje* self) {
    float divisor = *((float*)self->derecha.valor);
    if (divisor == 0.0f) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) / divisor;
    return nuevoValorResultado(res, FLOAT);
}

Result dividirDoubleDouble(ExpresionLenguaje* self) {
    double divisor = *((double*)self->derecha.valor);
    if (divisor == 0.0) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) / divisor;
    return nuevoValorResultado(res, DOUBLE);
}

Result dividirIntFloat(ExpresionLenguaje* self) {
    float divisor = *((float*)self->derecha.valor);
    if (divisor == 0.0f) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    float* res = malloc(sizeof(float));
    *res = (float)(*((int*)self->izquierda.valor)) / divisor;
    return nuevoValorResultado(res, FLOAT);
}

Result dividirFloatInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) / (float)divisor;
    return nuevoValorResultado(res, FLOAT);
}

Result dividirIntDouble(ExpresionLenguaje* self) {
    double divisor = *((double*)self->derecha.valor);
    if (divisor == 0.0) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    double* res = malloc(sizeof(double));
    *res = (double)(*((int*)self->izquierda.valor)) / divisor;
    return nuevoValorResultado(res, DOUBLE);
}

Result dividirDoubleInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) / (double)divisor;
    return nuevoValorResultado(res, DOUBLE);
}

Result dividirFloatDouble(ExpresionLenguaje* self) {
    double divisor = *((double*)self->derecha.valor);
    if (divisor == 0.0) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    double* res = malloc(sizeof(double));
    *res = (double)(*((float*)self->izquierda.valor)) / divisor;
    return nuevoValorResultado(res, DOUBLE);
}

Result dividirDoubleFloat(ExpresionLenguaje* self) {
    float divisor = *((float*)self->derecha.valor);
    if (divisor == 0.0f) {
        add_error_to_report("Semántico", "/", "División por cero.", self->base.line, self->base.column, NULL);
        return nuevoValorResultadoVacio();
    }
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) / (double)divisor;
    return nuevoValorResultado(res, DOUBLE);
}

Operacion tablaOperacionesDivision[TIPO_COUNT][TIPO_COUNT] = {
    [INT][INT] = dividirIntInt,
    [FLOAT][FLOAT] = dividirFloatFloat,
    [DOUBLE][DOUBLE] = dividirDoubleDouble,
    [INT][FLOAT] = dividirIntFloat,
    [FLOAT][INT] = dividirFloatInt,
    [INT][DOUBLE] = dividirIntDouble,
    [DOUBLE][INT] = dividirDoubleInt,
    [FLOAT][DOUBLE] = dividirFloatDouble,
    [DOUBLE][FLOAT] = dividirDoubleFloat,

    // Nuevas operaciones para CHAR
    [CHAR][CHAR] = dividirIntInt,
    [CHAR][INT] = dividirIntInt,
    [INT][CHAR] = dividirIntInt,
    [CHAR][FLOAT] = dividirFloatInt, // Izquierda se promueve a float
    [FLOAT][CHAR] = dividirFloatInt,
    [CHAR][DOUBLE] = dividirDoubleInt, // Izquierda se promueve a double
    [DOUBLE][CHAR] = dividirDoubleInt
};

AbstractExpresion* nuevoDivisionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, izquierda, derecha, line, column);
    expr->base.node_type = "Division";
    expr->tablaOperaciones = &tablaOperacionesDivision;
    return (AbstractExpresion*) expr;
}
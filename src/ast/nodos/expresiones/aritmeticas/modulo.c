#include "aritmeticas.h"
#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "context/result.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <math.h> // <<<< IMPORTANTE: Se necesita para fmod y fmodf

Result moduloIntInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) {
        add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL);
        return nuevoValorResultadoVacio();
    }
    int* res = malloc(sizeof(int));
    *res = *((int*)self->izquierda.valor) % divisor;
    return nuevoValorResultado(res, INT);
}

Result moduloFloatFloat(ExpresionLenguaje* self) {
    float divisor = *((float*)self->derecha.valor);
    if (divisor == 0.0f) {
        add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL);
        return nuevoValorResultadoVacio();
    }
    float* res = malloc(sizeof(float));
    *res = fmodf(*((float*)self->izquierda.valor), divisor);
    return nuevoValorResultado(res, FLOAT);
}

Result moduloDoubleDouble(ExpresionLenguaje* self) {
    double divisor = *((double*)self->derecha.valor);
    if (divisor == 0.0) {
        add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL);
        return nuevoValorResultadoVacio();
    }
    double* res = malloc(sizeof(double));
    *res = fmod(*((double*)self->izquierda.valor), divisor);
    return nuevoValorResultado(res, DOUBLE);
}

Result moduloIntFloat(ExpresionLenguaje* self) {
    float divisor = *((float*)self->derecha.valor);
    if (divisor == 0.0f) { add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL); return nuevoValorResultadoVacio(); }
    float* res = malloc(sizeof(float));
    *res = fmodf((float)(*((int*)self->izquierda.valor)), divisor);
    return nuevoValorResultado(res, FLOAT);
}

Result moduloFloatInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) { add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL); return nuevoValorResultadoVacio(); }
    float* res = malloc(sizeof(float));
    *res = fmodf(*((float*)self->izquierda.valor), (float)divisor);
    return nuevoValorResultado(res, FLOAT);
}

Result moduloIntDouble(ExpresionLenguaje* self) {
    double divisor = *((double*)self->derecha.valor);
    if (divisor == 0.0) { add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL); return nuevoValorResultadoVacio(); }
    double* res = malloc(sizeof(double));
    *res = fmod((double)(*((int*)self->izquierda.valor)), divisor);
    return nuevoValorResultado(res, DOUBLE);
}

Result moduloDoubleInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) { add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL); return nuevoValorResultadoVacio(); }
    double* res = malloc(sizeof(double));
    *res = fmod(*((double*)self->izquierda.valor), (double)divisor);
    return nuevoValorResultado(res, DOUBLE);
}

Result moduloFloatDouble(ExpresionLenguaje* self) {
    double divisor = *((double*)self->derecha.valor);
    if (divisor == 0.0) { add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL); return nuevoValorResultadoVacio(); }
    double* res = malloc(sizeof(double));
    *res = fmod((double)(*((float*)self->izquierda.valor)), divisor);
    return nuevoValorResultado(res, DOUBLE);
}

Result moduloDoubleFloat(ExpresionLenguaje* self) {
    float divisor = *((float*)self->derecha.valor);
    if (divisor == 0.0f) { add_error_to_report("Semántico", "%", "Módulo por cero.", 0, 0, NULL); return nuevoValorResultadoVacio(); }
    double* res = malloc(sizeof(double));
    *res = fmod(*((double*)self->izquierda.valor), (double)divisor);
    return nuevoValorResultado(res, DOUBLE);
}

Operacion tablaOperacionesModulo[TIPO_COUNT][TIPO_COUNT] = {
    [INT][INT] = moduloIntInt,
    [FLOAT][FLOAT] = moduloFloatFloat,
    [DOUBLE][DOUBLE] = moduloDoubleDouble,
    [INT][FLOAT] = moduloIntFloat,
    [FLOAT][INT] = moduloFloatInt,
    [INT][DOUBLE] = moduloIntDouble,
    [DOUBLE][INT] = moduloDoubleInt,
    [FLOAT][DOUBLE] = moduloFloatDouble,
    [DOUBLE][FLOAT] = moduloDoubleFloat
};

AbstractExpresion* nuevoModuloExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, izquierda, derecha);
    expr->base.node_type = "Modulo";
    expr->tablaOperaciones = &tablaOperacionesModulo;
    return (AbstractExpresion*) expr;
}
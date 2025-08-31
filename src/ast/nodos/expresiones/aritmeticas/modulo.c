#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "aritmeticas.h"
#include <stdlib.h>
#include <stdio.h>

Result moduloIntInt(ExpresionLenguaje* self) {
    int divisor = *((int*)self->derecha.valor);
    if (divisor == 0) {
        printf("Error: Módulo por cero.\n");
        return nuevoValorResultadoVacio();
    }
    int* res = malloc(sizeof(int));
    *res = *((int*)self->izquierda.valor) % divisor;
    return nuevoValorResultado(res, INT);
}

// El módulo generalmente no se define para números de punto flotante.

Operacion tablaOperacionesModulo[TIPO_COUNT][TIPO_COUNT] = {
    [INT][INT] = moduloIntInt,
};

AbstractExpresion* nuevoModuloExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, izquierda, derecha);
    expr->tablaOperaciones = &tablaOperacionesModulo;
    return (AbstractExpresion*) expr;
}
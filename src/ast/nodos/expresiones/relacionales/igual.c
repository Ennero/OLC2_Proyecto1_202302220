#include "ast/nodos/expresiones/expresiones.h"
#include "ast/nodos/builders.h"
#include "context/result.h"
#include <stdlib.h>
#include <string.h>

// El resultado es siempre un booleano (1 para true, 0 para false)
static Result compararNumeros(ExpresionLenguaje* self, int negate) {
    int* res = malloc(sizeof(int));
    double v1 = (self->izquierda.tipo == INT) ? (double)(*(int*)self->izquierda.valor) : *((double*)self->izquierda.valor);
    double v2 = (self->derecha.tipo == INT) ? (double)(*(int*)self->derecha.valor) : *((double*)self->derecha.valor);
    *res = (v1 == v2);
    if (negate) *res = !(*res);
    return nuevoValorResultado(res, BOOLEAN);
}

static Result compararStrings(ExpresionLenguaje* self, int negate) {
    int* res = malloc(sizeof(int));
    *res = (strcmp((char*)self->izquierda.valor, (char*)self->derecha.valor) == 0);
    if (negate) *res = !(*res);
    return nuevoValorResultado(res, BOOLEAN);
}

static Result compararBooleanos(ExpresionLenguaje* self, int negate) {
    int* res = malloc(sizeof(int));
    *res = (*(int*)self->izquierda.valor == *(int*)self->derecha.valor);
    if (negate) *res = !(*res);
    return nuevoValorResultado(res, BOOLEAN);
}

// Funciones wrapper para no duplicar código
static Result compararIgual(ExpresionLenguaje* self) { return compararNumeros(self, 0); }
static Result compararStringsIgual(ExpresionLenguaje* self) { return compararStrings(self, 0); }
static Result compararBooleanosIgual(ExpresionLenguaje* self) { return compararBooleanos(self, 0); }

static Result compararDiferente(ExpresionLenguaje* self) { return compararNumeros(self, 1); }
static Result compararStringsDiferente(ExpresionLenguaje* self) { return compararStrings(self, 1); }
static Result compararBooleanosDiferente(ExpresionLenguaje* self) { return compararBooleanos(self, 1); }


Operacion tablaOperacionesIgual[TIPO_COUNT][TIPO_COUNT] = {
    [INT]    = { [INT]=compararIgual, [DOUBLE]=compararIgual, [CHAR]=compararIgual },
    [DOUBLE] = { [INT]=compararIgual, [DOUBLE]=compararIgual, [CHAR]=compararIgual },
    [CHAR]   = { [INT]=compararIgual, [DOUBLE]=compararIgual, [CHAR]=compararIgual },
    [STRING] = { [STRING]=compararStringsIgual },
    [BOOLEAN]= { [BOOLEAN]=compararBooleanosIgual }
};

Operacion tablaOperacionesDiferente[TIPO_COUNT][TIPO_COUNT] = {
    [INT]    = { [INT]=compararDiferente, [DOUBLE]=compararDiferente, [CHAR]=compararDiferente },
    [DOUBLE] = { [INT]=compararDiferente, [DOUBLE]=compararDiferente, [CHAR]=compararDiferente },
    [CHAR]   = { [INT]=compararDiferente, [DOUBLE]=compararDiferente, [CHAR]=compararDiferente },
    [STRING] = { [STRING]=compararStringsDiferente },
    [BOOLEAN]= { [BOOLEAN]=compararBooleanosDiferente }
};

AbstractExpresion* nuevoIgualExpresion(AbstractExpresion* i, AbstractExpresion* d, int line, int column) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, i, d, line, column);
    expr->base.node_type = "IgualIgual";
    expr->tablaOperaciones = &tablaOperacionesIgual;
    return (AbstractExpresion*) expr;
}

AbstractExpresion* nuevoDiferenteExpresion(AbstractExpresion* i, AbstractExpresion* d, int line, int column) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, i, d, line, column);
    expr->base.node_type = "Diferente";
    expr->tablaOperaciones = &tablaOperacionesDiferente;
    return (AbstractExpresion*) expr;
}
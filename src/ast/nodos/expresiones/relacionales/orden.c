#include "ast/nodos/expresiones/expresiones.h"
#include "ast/nodos/builders.h"
#include "context/result.h"
#include <stdlib.h>

// Función genérica para comparar orden (>, <, >=, <=)
static Result compararOrden(ExpresionLenguaje* self, int op_type) {
    int* res = malloc(sizeof(int));
    double v1 = (self->izquierda.tipo == INT) ? (double)(*(int*)self->izquierda.valor) : *((double*)self->izquierda.valor);
    double v2 = (self->derecha.tipo == INT) ? (double)(*(int*)self->derecha.valor) : *((double*)self->derecha.valor);
    
    switch(op_type) {
        case 0: *res = (v1 > v2); break;  // Mayor que
        case 1: *res = (v1 < v2); break;  // Menor que
        case 2: *res = (v1 >= v2); break; // Mayor o igual
        case 3: *res = (v1 <= v2); break; // Menor o igual
    }
    return nuevoValorResultado(res, BOOLEAN);
}

// Wrappers para cada operador
static Result compararMayorQue(ExpresionLenguaje* self) { return compararOrden(self, 0); }
static Result compararMenorQue(ExpresionLenguaje* self) { return compararOrden(self, 1); }
static Result compararMayorIgual(ExpresionLenguaje* self) { return compararOrden(self, 2); }
static Result compararMenorIgual(ExpresionLenguaje* self) { return compararOrden(self, 3); }

// Tablas de operaciones (todas usan la misma lógica)
Operacion tablaOperacionesMayorQue[TIPO_COUNT][TIPO_COUNT] = {
    [INT]={[INT]=compararMayorQue,[DOUBLE]=compararMayorQue,[CHAR]=compararMayorQue},
    [DOUBLE]={[INT]=compararMayorQue,[DOUBLE]=compararMayorQue,[CHAR]=compararMayorQue},
    [CHAR]={[INT]=compararMayorQue,[DOUBLE]=compararMayorQue,[CHAR]=compararMayorQue}
};
Operacion tablaOperacionesMenorQue[TIPO_COUNT][TIPO_COUNT] = {
    [INT]={[INT]=compararMenorQue,[DOUBLE]=compararMenorQue,[CHAR]=compararMenorQue},
    [DOUBLE]={[INT]=compararMenorQue,[DOUBLE]=compararMenorQue,[CHAR]=compararMenorQue},
    [CHAR]={[INT]=compararMenorQue,[DOUBLE]=compararMenorQue,[CHAR]=compararMenorQue}
};
Operacion tablaOperacionesMayorIgual[TIPO_COUNT][TIPO_COUNT] = {
    [INT]={[INT]=compararMayorIgual,[DOUBLE]=compararMayorIgual,[CHAR]=compararMayorIgual},
    [DOUBLE]={[INT]=compararMayorIgual,[DOUBLE]=compararMayorIgual,[CHAR]=compararMayorIgual},
    [CHAR]={[INT]=compararMayorIgual,[DOUBLE]=compararMayorIgual,[CHAR]=compararMayorIgual}
};
Operacion tablaOperacionesMenorIgual[TIPO_COUNT][TIPO_COUNT] = {
    [INT]={[INT]=compararMenorIgual,[DOUBLE]=compararMenorIgual,[CHAR]=compararMenorIgual},
    [DOUBLE]={[INT]=compararMenorIgual,[DOUBLE]=compararMenorIgual,[CHAR]=compararMenorIgual},
    [CHAR]={[INT]=compararMenorIgual,[DOUBLE]=compararMenorIgual,[CHAR]=compararMenorIgual}
};

// Constructores de Nodos
AbstractExpresion* nuevoMayorQueExpresion(AbstractExpresion* i, AbstractExpresion* d) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, i, d);
    expr->base.node_type = "MayorQue";
    expr->tablaOperaciones = &tablaOperacionesMayorQue;
    return (AbstractExpresion*) expr;
}
AbstractExpresion* nuevoMenorQueExpresion(AbstractExpresion* i, AbstractExpresion* d) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, i, d);
    expr->base.node_type = "MenorQue";
    expr->tablaOperaciones = &tablaOperacionesMenorQue;
    return (AbstractExpresion*) expr;
}
AbstractExpresion* nuevoMayorIgualExpresion(AbstractExpresion* i, AbstractExpresion* d) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, i, d);
    expr->base.node_type = "MayorIgual";
    expr->tablaOperaciones = &tablaOperacionesMayorIgual;
    return (AbstractExpresion*) expr;
}
AbstractExpresion* nuevoMenorIgualExpresion(AbstractExpresion* i, AbstractExpresion* d) {
    ExpresionLenguaje* expr = nuevoExpresionLenguaje(interpretExpresionLenguaje, i, d);
    expr->base.node_type = "MenorIgual";
    expr->tablaOperaciones = &tablaOperacionesMenorIgual;
    return (AbstractExpresion*) expr;
}
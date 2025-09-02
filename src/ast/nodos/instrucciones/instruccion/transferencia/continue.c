// src/.../transferencia/continue.c
#include "ast/nodos/builders.h"
#include <stdlib.h>

// El 'continue' simplemente devuelve un resultado especial
Result interpretContinueExpresion(AbstractExpresion* self, Context* context) {
    (void)self; (void)context; // No se usan los parámetros
    return nuevoValorResultado(NULL, CONTINUE_T);
}

AbstractExpresion* nuevoContinueExpresion(int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretContinueExpresion, "ContinueStatement", line, column);
    return nodo;
}
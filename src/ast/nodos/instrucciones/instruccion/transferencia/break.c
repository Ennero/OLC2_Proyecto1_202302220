#include "ast/nodos/builders.h"
#include <stdlib.h>

// El 'break' simplemente devuelve un resultado especial
Result interpretBreakExpresion(AbstractExpresion* self, Context* context) {
    (void)self; (void)context; // No se usan los parámetros
    return nuevoValorResultado(NULL, BREAK_T);
}

AbstractExpresion* nuevoBreakExpresion(int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretBreakExpresion, "BreakStatement", line, column);
    return nodo;
}
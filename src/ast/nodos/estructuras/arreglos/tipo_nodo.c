// en: src/ast/nodos/estructuras/arreglos/tipo_nodo.c
#include "ast/AbstractExpresion.h"
#include <stdlib.h>

// Representación segura de un nodo de tipo para arreglos
typedef struct {
    AbstractExpresion base;
    TipoDato tipo;
} TypeNodeExpresion;

// Devuelve el tipo deseado en Result.tipo; no usa Result.valor
Result interpretTipoNode(AbstractExpresion* self, Context* context) {
    (void)context;
    TypeNodeExpresion* nodo = (TypeNodeExpresion*)self;
    return nuevoValorResultado(NULL, nodo->tipo);
}

AbstractExpresion* nuevoTipoNode(TipoDato tipo) {
    TypeNodeExpresion* nodo = malloc(sizeof(TypeNodeExpresion));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretTipoNode, "TypeNode", 0, 0);
    nodo->tipo = tipo;
    return (AbstractExpresion*)nodo;
}
#include "parametro.h"
#include "ast/nodos/builders.h"
#include <stdlib.h>

Result interpretParametro(AbstractExpresion* self, Context* context) {
    // Un nodo de parámetro no se "interpreta", solo contiene datos.
    (void)self; (void)context;
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoParametro(TipoDato tipo, char* nombre) {
    ParametroNode* nodo = malloc(sizeof(ParametroNode));
    buildAbstractExpresion(&nodo->base, interpretParametro, "Parametro", 0, 0);
    nodo->tipo = tipo;
    nodo->nombre = nombre;
    return (AbstractExpresion*)nodo;
}
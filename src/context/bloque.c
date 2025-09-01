#include "bloque.h"
#include "error_reporter.h"
#include <stdlib.h>
#include "ast/nodos/builders.h"

// La función de interpretación ahora crea un contexto con nombre
Result interpretBloque(AbstractExpresion* self, Context* context) {
    Context* nuevo = nuevoContext(context, "bloque");

    if (self->numHijos > 0) {
        AbstractExpresion* instrucciones = self->hijos[0];
        instrucciones->interpret(instrucciones, nuevo);
        if (has_semantic_error_been_found()) {
            return nuevoValorResultadoVacio();
        }
    }

    // Aquí se liberaría la memoria del 'nuevo' contexto y sus símbolos
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* instrucciones, int line, int column) {
    BloqueExpresion* nodo = malloc(sizeof(BloqueExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretBloque, "Bloque", line, column);
    if (instrucciones) {
        agregarHijo((AbstractExpresion*)nodo, instrucciones);
    }
    return (AbstractExpresion*) nodo;
}

#include "bloque.h"
#include <stdlib.h>
#include "ast/nodos/builders.h"

// La función de interpretación ahora crea un contexto con nombre
Result interpretBloque(AbstractExpresion* self, Context* context) {
    // <<<<<<< CAMBIO: Se pasa "bloque" como el tipo de contexto
    Context* nuevo = nuevoContext(context, "bloque");

    if (self->numHijos > 0) {
        AbstractExpresion* instrucciones = self->hijos[0];
        instrucciones->interpret(instrucciones, nuevo);
    }

    // Aquí se liberaría la memoria del 'nuevo' contexto y sus símbolos
    // (Implementación futura)
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* instrucciones) {
    BloqueExpresion* nodo = malloc(sizeof(BloqueExpresion));
    if (!nodo) return NULL;
    
    buildAbstractExpresion(&nodo->base, interpretBloque);
    if (instrucciones) {
        agregarHijo((AbstractExpresion*)nodo, instrucciones);
    }
    return (AbstractExpresion*) nodo;
}

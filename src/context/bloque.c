#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "bloque.h"
#include <stdlib.h>

// ESTA ES LA FUNCIÓN CLAVE
Result interpretBloque(AbstractExpresion* self, Context* context) {
    // 1. Crear un nuevo contexto para este bloque. Su padre es el contexto actual.
    Context* new_context = nuevoContext(context);

    // 2. El bloque tiene un solo hijo: la lista de instrucciones (lSentencia).
    //    Lo interpretamos usando el NUEVO contexto.
    if (self->numHijos > 0) {
        self->hijos[0]->interpret(self->hijos[0], new_context);
    }
    
    // 3. Al terminar la función, 'new_context' se pierde y volvemos
    //    automáticamente al contexto padre. ¡El ámbito se cierra!
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* sentencias) {
    BloqueExpresion* nodo = malloc(sizeof(BloqueExpresion));
    if (!nodo) return NULL;

    // Asignamos la función de interpretación específica para bloques.
    buildAbstractExpresion(&nodo->base, interpretBloque);

    // El hijo de un nodo de bloque es la lista de sentencias que contiene.
    if (sentencias) {
        agregarHijo((AbstractExpresion*)nodo, sentencias);
    }
    
    return (AbstractExpresion*)nodo;
}
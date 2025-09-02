#include "bloque.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include <stdlib.h>
#include <stdio.h> // Necesario para sprintf

// La función que interpreta un bloque de sentencias
Result interpretBloqueExpresion(AbstractExpresion* self, Context* context) {
    // <<<< LÓGICA MODIFICADA >>>>
    // 1. Obtenemos el contexto raíz para acceder al contador global
    Context* raiz = context->raiz;

    // 2. Generar el nombre para el nuevo bloque usando el contador de la RAÍZ
    char nombre_buffer[32];
    sprintf(nombre_buffer, "bloque%d", raiz->proximo_id_bloque);
    
    // 3. Incrementar el contador en la RAÍZ para el siguiente bloque en cualquier parte del programa
    raiz->proximo_id_bloque++;

    // 4. Crear el nuevo contexto para este bloque, pasándole el nombre único
    Context* nuevo_contexto = nuevoContext(context, nombre_buffer);
    
    // 5. Interpretar las sentencias hijas (si las hay) dentro del NUEVO contexto
    if (self->hijos[0]) {
        self->hijos[0]->interpret(self->hijos[0], nuevo_contexto);
    }
    
    // 6. Liberar el nuevo contexto y todos sus símbolos
    liberarContext(nuevo_contexto);

    return nuevoValorResultadoVacio();
}

// El constructor del nodo de bloque
AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* instrucciones, int line, int column) {
    BloqueExpresion* nodo = malloc(sizeof(BloqueExpresion));
    if (!nodo) return NULL;

    // (MODIFICADO) Aseguramos que el constructor use el nombre correcto de la función.
    buildAbstractExpresion(&nodo->base, interpretBloqueExpresion, "Bloque", line, column);
    
    if (instrucciones) {
        agregarHijo((AbstractExpresion*)nodo, instrucciones);
    }
    return (AbstractExpresion*) nodo;
}
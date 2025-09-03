#include "bloque.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include <stdlib.h>
#include <stdio.h>

// La función que interpreta un bloque de sentencias (CORREGIDA)
Result interpretBloqueExpresion(AbstractExpresion* self, Context* context) {
    // 1. Generar el nombre para el nuevo bloque
    char nombre_buffer[32];
    sprintf(nombre_buffer, "bloque%d", context->raiz->proximo_id_bloque);
    context->raiz->proximo_id_bloque++;

    // 2. Crear el nuevo contexto para este bloque
    Context* nuevo_contexto = nuevoContext(context, nombre_buffer);
    
    // 3. (CAMBIO CLAVE) Interpretar las sentencias y CAPTURAR su resultado
    Result res_bloque = nuevoValorResultadoVacio(); // Valor por defecto si el bloque está vacío
    if (self->hijos[0]) {
        res_bloque = self->hijos[0]->interpret(self->hijos[0], nuevo_contexto);
    }
    
    // 4. Liberar el nuevo contexto
    liberarContext(nuevo_contexto);

    // 5. (CAMBIO CLAVE) Devolver el resultado capturado del bloque.
    // Si hubo un return, res_bloque contendrá la señal RETURN_T.
    return res_bloque;
}

// El constructor del nodo de bloque (sin cambios)
AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* lSentencia, int line, int column) {
    BloqueExpresion* nodo = malloc(sizeof(BloqueExpresion));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretBloqueExpresion, "Bloque", line, column);
    if(lSentencia) {
        agregarHijo((AbstractExpresion*)nodo, lSentencia);
    }
    return (AbstractExpresion*)nodo;
}
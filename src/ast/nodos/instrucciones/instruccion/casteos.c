#include "casteos.h"
#include "ast/nodos/builders.h"
#include "context/result.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Prototipo de la función de interpretación
Result interpretCasteoExpresion(AbstractExpresion* self, Context* context);

// Implementación del constructor
AbstractExpresion* nuevoCasteoExpresion(TipoDato tipo_destino, AbstractExpresion* expresion, int line, int column) {
    CasteoExpresion* nodo = malloc(sizeof(CasteoExpresion));
    if (!nodo) {
        perror("No se pudo alocar memoria para CasteoExpresion");
        return NULL;
    }

    buildAbstractExpresion(&nodo->base, interpretCasteoExpresion, "Casteo", line, column);
    nodo->tipo_destino = tipo_destino;
    agregarHijo((AbstractExpresion*)nodo, expresion);

    return (AbstractExpresion*)nodo;
}

// Implementación de la función de interpretación
Result interpretCasteoExpresion(AbstractExpresion* self, Context* context) {
    CasteoExpresion* nodo_casteo = (CasteoExpresion*)self;
    AbstractExpresion* expresion_a_castear = self->hijos[0];

    // 1. Interpretar la expresión interna para obtener el valor original
    Result resultado_original = expresion_a_castear->interpret(expresion_a_castear, context);

    if (resultado_original.tipo == NULO) {
        // No se puede castear un valor nulo o erróneo
        return resultado_original;
    }

    TipoDato tipo_origen = resultado_original.tipo;
    TipoDato tipo_destino = nodo_casteo->tipo_destino;

    if (tipo_origen == tipo_destino) {
        // No es necesario castear, devolver el resultado original
        return resultado_original;
    }

    // 2. Lógica de conversión de tipos
    // TODO: Implementar las conversiones específicas.
    // Por ahora, un ejemplo simple:
    char description[256];
    sprintf(description, "Casteo de %s a %s no implementado.", labelTipoDato[tipo_origen], labelTipoDato[tipo_destino]);
    add_error_to_report("Semantico", "casteo", description, self->line, self->column, context->nombre_completo);

    return nuevoValorResultadoVacio();
}

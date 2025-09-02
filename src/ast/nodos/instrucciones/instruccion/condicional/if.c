#include "if.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

Result interpretIfExpresion(AbstractExpresion* self, Context* context) {
    // 1. Interpretar la condición
    Result res_cond = self->hijos[0]->interpret(self->hijos[0], context);

    if (has_semantic_error_been_found()) {
        free(res_cond.valor);
        return nuevoValorResultadoVacio();
    }

    // 2. Comprobar que la condición sea de tipo booleano
    if (res_cond.tipo != BOOLEAN) {
        char desc[256];
        snprintf(desc, sizeof(desc), "Se esperaba una expresión booleana en la condición del if, pero se encontró un valor de tipo '%s'.", labelTipoDato[res_cond.tipo]);
        add_error_to_report("Semantico", "if", desc, self->line, self->column, context->nombre_completo);
        free(res_cond.valor);
        return nuevoValorResultadoVacio();
    }
    
    // Obtenemos el valor booleano (0 o 1)
    int condicion_val = *(int*)res_cond.valor;

    // Liberamos la memoria del resultado temporal de la condición
    free(res_cond.valor);

    // 3. Ejecutar el bloque correspondiente
    if (condicion_val) {
        // La condición es verdadera, ejecutar el bloque "if"
        return self->hijos[1]->interpret(self->hijos[1], context);
    } else {
        // La condición es falsa, comprobar si hay un bloque "else"
        if (self->hijos[2] != NULL) {
            // Ejecutar el bloque "else" (que puede ser otro if_stmt o un bloque final)
            return self->hijos[2]->interpret(self->hijos[2], context);
        }
    }

    // Si no se cumple la condición y no hay else, no se hace nada.
    return nuevoValorResultadoVacio();
}

// Constructor para el nodo IF
AbstractExpresion* nuevoIfExpresion(AbstractExpresion* condicion, AbstractExpresion* bloque_if, AbstractExpresion* bloque_else, int line, int column) {
    IfExpresion* nodo = malloc(sizeof(IfExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretIfExpresion, "IfStatement", line, column);
    
    agregarHijo((AbstractExpresion*)nodo, condicion);
    agregarHijo((AbstractExpresion*)nodo, bloque_if);
    
    // El bloque else es opcional
    if (bloque_else) {
        agregarHijo((AbstractExpresion*)nodo, bloque_else);
    }
    
    return (AbstractExpresion*)nodo;
}
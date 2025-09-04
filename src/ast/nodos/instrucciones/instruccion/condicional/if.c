#include "if.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

Result interpretIfExpresion(AbstractExpresion* self, Context* context) {
    // 1. Interpretar la condición
    Result res_cond = self->hijos[0]->interpret(self->hijos[0], context);

    // Comprobaciones de error semántico y de tipo (esto ya estaba bien)
    if (has_semantic_error_been_found()) {
        free(res_cond.valor);
        return nuevoValorResultadoVacio();
    }
    if (res_cond.tipo != BOOLEAN) {
        char desc[256];
        snprintf(desc, sizeof(desc), "Se esperaba una expresión booleana en la condición del if, pero se encontró un valor de tipo '%s'.", labelTipoDato[res_cond.tipo]);
        add_error_to_report("Semantico", "if", desc, self->line, self->column, context->nombre_completo);
        free(res_cond.valor);
        return nuevoValorResultadoVacio();
    }
    
    int condicion_val = *(int*)res_cond.valor;
    free(res_cond.valor);

    // --- INICIO DE LA LÓGICA CORREGIDA Y ROBUSTA ---

    Result resultado_bloque = nuevoValorResultadoVacio();

    // 2. Ejecutar el bloque correspondiente si la condición se cumple
    if (condicion_val) {
        // El hijo 1 es el bloque del 'if'
        resultado_bloque = self->hijos[1]->interpret(self->hijos[1], context);
    } else if (self->numHijos > 2 && self->hijos[2] != NULL) {
        // El hijo 2 es el bloque 'else' (que podría ser otro 'if')
        resultado_bloque = self->hijos[2]->interpret(self->hijos[2], context);
    }

    // 3. El 'if' como sentencia toma posesión del resultado del bloque.
    //    Su única responsabilidad es propagar las señales de control.
    if (resultado_bloque.tipo == BREAK_T || 
        resultado_bloque.tipo == CONTINUE_T || 
        resultado_bloque.tipo == RETURN_T) {
        // Propaga la señal hacia arriba.
        return resultado_bloque;
    }

    // 4. Si el resultado del bloque no fue una señal, su valor ya no es necesario.
    //    El 'if' lo descarta y limpia la memoria.
    free(resultado_bloque.valor);

    // La sentencia 'if' como tal no produce ningún valor.
    return nuevoValorResultadoVacio();
    // --- FIN DE LA LÓGICA CORREGIDA ---
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
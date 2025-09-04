// en: src/ast/nodos/instrucciones/instruccion/ciclo/while.c

#include "while.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// Prototipo de la función de interpretación
Result interpretWhileExpresion(AbstractExpresion* self, Context* context);

// Implementación del constructor
AbstractExpresion* nuevoWhileExpresion(AbstractExpresion* condicion, AbstractExpresion* bloque, int line, int column) {
    WhileExpresion* nodo = malloc(sizeof(WhileExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretWhileExpresion, "WhileStatement", line, column);

    // El nodo 'while' tiene dos hijos: la condición y el bloque de código
    agregarHijo((AbstractExpresion*)nodo, condicion);
    agregarHijo((AbstractExpresion*)nodo, bloque);

    return (AbstractExpresion*)nodo;
}

Result interpretWhileExpresion(AbstractExpresion* self, Context* context) {
    AbstractExpresion* condicion_nodo = self->hijos[0];
    AbstractExpresion* bloque_nodo = self->hijos[1];

    while (1) {
        // 1. Evaluar la condición
        Result res_cond = condicion_nodo->interpret(condicion_nodo, context);

        // Comprobaciones de error semántico y de tipo (esto ya estaba bien)
        if (has_semantic_error_been_found()) {
            free(res_cond.valor);
            return nuevoValorResultadoVacio();
        }
        if (res_cond.tipo != BOOLEAN) {
            char desc[256];
            snprintf(desc, sizeof(desc), "Se esperaba una expresión booleana en la condición del while, pero se encontró un valor de tipo '%s'.", labelTipoDato[res_cond.tipo]);
            add_error_to_report("Semantico", "while", desc, self->line, self->column, context->nombre_completo);
            free(res_cond.valor);
            return nuevoValorResultadoVacio();
        }

        int condicion_val = *(int*)res_cond.valor;
        free(res_cond.valor);

        // Si la condición es falsa, el bucle termina
        if (!condicion_val) {
            break;
        }

        // 2. Ejecutar el bloque
        Result res_bloque = bloque_nodo->interpret(bloque_nodo, context);

        // --- INICIO DE LA LÓGICA CORREGIDA Y A PRUEBA DE BALAS ---

        // Si el bloque nos pide retornar de la función, lo propagamos inmediatamente.
        // El valor de retorno NO se libera, se está pasando hacia arriba.
        if (res_bloque.tipo == RETURN_T) {
            return res_bloque;
        }

        // Guardamos el tipo de señal que recibimos (si la hay) antes de tocar la memoria.
        TipoDato tipo_señal = res_bloque.tipo;

        // Liberamos el valor del resultado del bloque, SEA LO QUE SEA.
        // free(NULL) es una operación segura que no hace nada. Esto limpia cualquier
        // resultado temporal que el bloque pudiera haber devuelto si no fue una señal.
        free(res_bloque.valor);

        // Ahora, y solo ahora, actuamos según la señal (si la hubo).
        if (tipo_señal == BREAK_T) {
            break; // Salimos del bucle while.
        }
        if (tipo_señal == CONTINUE_T) {
            continue; // Vamos a la siguiente iteración.
        }
        // Si no fue ninguna señal, la iteración simplemente termina y comienza la siguiente.
        // --- FIN DE LA LÓGICA CORREGIDA ---
    }

    return nuevoValorResultadoVacio();
}
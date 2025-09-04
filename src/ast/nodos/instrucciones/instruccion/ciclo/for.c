// en: src/ast/nodos/instrucciones/instruccion/ciclo/for.c

#include "for.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

Result interpretForExpresion(AbstractExpresion* self, Context* context);

AbstractExpresion* nuevoForExpresion(AbstractExpresion* init, AbstractExpresion* cond, AbstractExpresion* update, AbstractExpresion* bloque, int line, int column) {
    ForExpresion* nodo = malloc(sizeof(ForExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretForExpresion, "ForStatement", line, column);

    // Los 4 hijos pueden ser NULL, representando las partes opcionales del for
    agregarHijo((AbstractExpresion*)nodo, init);
    agregarHijo((AbstractExpresion*)nodo, cond);
    agregarHijo((AbstractExpresion*)nodo, update);
    agregarHijo((AbstractExpresion*)nodo, bloque);

    return (AbstractExpresion*)nodo;
}

Result interpretForExpresion(AbstractExpresion* self, Context* context) {
    AbstractExpresion* init_nodo = self->hijos[0];
    AbstractExpresion* cond_nodo = self->hijos[1];
    AbstractExpresion* update_nodo = self->hijos[2];
    AbstractExpresion* bloque_nodo = self->hijos[3];

    // 1. Crear un nuevo contexto para el ámbito del ciclo 'for'
    Context* for_context = nuevoContext(context, "for_loop");

    // 2. Ejecutar la inicialización UNA VEZ dentro del nuevo contexto
    if (init_nodo) {
        Result res_init = init_nodo->interpret(init_nodo, for_context);
        free(res_init.valor); // El resultado de la inicialización no se usa
    }

    while (1) {
        // 3. Evaluar la condición
        bool condicion_val = true; // Por defecto, un 'for' sin condición es un bucle infinito
        if (cond_nodo) {
            Result res_cond = cond_nodo->interpret(cond_nodo, for_context);
            if (res_cond.tipo != BOOLEAN) {
                // ... (manejo de error si la condición no es booleana) ...
                free(res_cond.valor);
                condicion_val = false; // Detener el bucle en caso de error
            } else {
                condicion_val = *(int*)res_cond.valor;
                free(res_cond.valor);
            }
        }
        
        if (!condicion_val) {
            break; // Salir del bucle
        }

        // 4. Ejecutar el bloque del cuerpo
        Result res_bloque = bloque_nodo->interpret(bloque_nodo, for_context);

        if (res_bloque.tipo == RETURN_T) {
            liberarContext(for_context); // Liberar el contexto antes de salir
            return res_bloque;
        }

        TipoDato tipo_señal = res_bloque.tipo;
        free(res_bloque.valor);

        if (tipo_señal == BREAK_T) {
            break; // Salir del bucle
        }
        if (tipo_señal == CONTINUE_T) {
            // Un 'continue' salta directamente al paso de actualización
            goto update_step;
        }

    update_step:; // Etiqueta para el 'continue'
        // 5. Ejecutar la actualización
        if (update_nodo) {
            Result res_update = update_nodo->interpret(update_nodo, for_context);
            free(res_update.valor); // El resultado de la actualización no se usa
        }
    }

    // 6. Liberar el contexto del 'for' al terminar el bucle
    liberarContext(for_context);

    return nuevoValorResultadoVacio();
}
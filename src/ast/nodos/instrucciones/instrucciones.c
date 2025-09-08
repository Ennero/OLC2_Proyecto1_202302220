#include "instrucciones.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "error_reporter.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Result interpretInstruccionesExpresion(AbstractExpresion* self, Context* context) {
    // Dos fases SOLO en nivel superior (cuando hay funciones/main en esta lista).
    // Dentro de bloques, ejecutar en orden para respetar dependencias (p. ej., asignar antes de usar).

    bool has_fn_like = false;
    for (size_t i = 0; i < self->numHijos; ++i) {
        const char* t = self->hijos[i]->node_type;
        if (t && (strcmp(t, "FunctionDeclaration") == 0 || strcmp(t, "MainFunction") == 0)) {
            has_fn_like = true; break;
        }
    }

    Result res = nuevoValorResultadoVacio();

    if (has_fn_like) {
        // Fase 1: solo Declaracion (variables globales) y FunctionDeclaration
        for (size_t i = 0; i < self->numHijos; ++i) {
            AbstractExpresion* nodo = self->hijos[i];
            const char* t = nodo->node_type;
            if (t && (strcmp(t, "Declaracion") == 0 || strcmp(t, "FunctionDeclaration") == 0)) {
                if (res.valor) { free(res.valor); res = nuevoValorResultadoVacio(); }
                res = nodo->interpret(nodo, context);
                if (has_semantic_error_been_found()) {
                    return res; // error ya reportado
                }
            }
        }

        // Fase 2: ejecutar el resto (p. ej., MainFunction y otras sentencias sueltas en toplevel)
        for (size_t i = 0; i < self->numHijos; ++i) {
            AbstractExpresion* nodo = self->hijos[i];
            const char* t = nodo->node_type;
            if (!(t && (strcmp(t, "Declaracion") == 0 || strcmp(t, "FunctionDeclaration") == 0))) {
                if (res.valor) { free(res.valor); res = nuevoValorResultadoVacio(); }
                res = nodo->interpret(nodo, context);
                if (res.tipo == BREAK_T || res.tipo == CONTINUE_T || res.tipo == RETURN_T) {
                    return res;
                }
                if (has_semantic_error_been_found()) {
                    return res; // error ya reportado
                }
            }
        }
    } else {
        // Ejecución secuencial dentro de bloques normales
        for (size_t i = 0; i < self->numHijos; ++i) {
            if (res.valor) { free(res.valor); res = nuevoValorResultadoVacio(); }
            res = self->hijos[i]->interpret(self->hijos[i], context);
            if (res.tipo == BREAK_T || res.tipo == CONTINUE_T || res.tipo == RETURN_T) {
                return res;
            }
            if (has_semantic_error_been_found()) {
                return res; // error ya reportado
            }
        }
    }

    if (res.valor) { free(res.valor); }
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoInstruccionesExpresion() {
    InstruccionesExpresion* nodo = malloc(sizeof(InstruccionesExpresion));
    if (!nodo) return NULL;

    // <<<< CORRECCIÓN DEL TYPO AQUÍ >>>>
    buildAbstractExpresion(&nodo->base, interpretInstruccionesExpresion, "Instrucciones", 0, 0);

    return (AbstractExpresion*) nodo;
}

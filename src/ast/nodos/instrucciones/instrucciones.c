#include "instrucciones.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "error_reporter.h"

#include <stdlib.h>
#include <stdio.h>

Result interpretInstruccionesExpresion(AbstractExpresion* self, Context* context) {
    Result res = nuevoValorResultadoVacio();

    for (size_t i = 0; i < self->numHijos; ++i) {
        // Liberamos el resultado de la iteración anterior para evitar fugas
        if(i > 0) free(res.valor); 

        res = self->hijos[i]->interpret(self->hijos[i], context);

        if (res.tipo == BREAK_T || res.tipo == CONTINUE_T || res.tipo == RETURN_T) {
            return res;
        }
        if (has_semantic_error_been_found()) {
            // El 'res' de una expresión errónea ya tiene valor NULL, no hay que liberarlo.
            return res;
        }
    }

    // Liberamos el resultado de la última instrucción válida
    free(res.valor);

    // Un bloque de instrucciones como tal no devuelve valor
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoInstruccionesExpresion() {
    InstruccionesExpresion* nodo = malloc(sizeof(InstruccionesExpresion));
    if (!nodo) return NULL;

    // <<<< CORRECCIÓN DEL TYPO AQUÍ >>>>
    buildAbstractExpresion(&nodo->base, interpretInstruccionesExpresion, "Instrucciones", 0, 0);

    return (AbstractExpresion*) nodo;
}

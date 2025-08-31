#include "instrucciones.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "error_reporter.h" // <<<< Se incluye para poder consultar la bandera de error

#include <stdlib.h>
#include <stdio.h>

Result interpretInstrucciones(AbstractExpresion* self, Context* context) {
    // <<<< LÓGICA DE DETENCIÓN SEMÁNTICA >>>>
    // Este bucle ahora comprueba la bandera de error antes de ejecutar cada instrucción.
    for (size_t i = 0; i < self->numHijos; ++i) {
        // Si en una iteración anterior (p. ej. al interpretar self->hijos[i-1])
        // se encontró un error semántico, la bandera se habrá activado.
        if (has_semantic_error_been_found()) {
            // Se detiene la ejecución de las siguientes instrucciones.
            return nuevoValorResultadoVacio();
        }
        // Si no hay error, se interpreta la instrucción actual.
        self->hijos[i]->interpret(self->hijos[i], context);
    }
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoInstruccionesExpresion() {
    InstruccionesExpresion* nodo = malloc(sizeof(InstruccionesExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretInstrucciones, "Instrucciones");

    return (AbstractExpresion*) nodo;
}


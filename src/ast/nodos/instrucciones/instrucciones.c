#include "instrucciones.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "error_reporter.h"

#include <stdlib.h>
#include <stdio.h>

Result interpretInstrucciones(AbstractExpresion* self, Context* context) {
    // Comprueba la bandera de error antes de ejecutar cada instrucción
    for (size_t i = 0; i < self->numHijos; ++i) {

        // Si en una iteración anterior se encontró un error semántico, la bandera se habrá activado.
        if (has_semantic_error_been_found()) {

            // Se detiene la ejecución.
            return nuevoValorResultadoVacio();
        }

        // Si no hay error, se interpreta la instrucción actual.
        self->hijos[i]->interpret(self->hijos[i], context);
    }

    // Si se llegó aquí, significa que no hubo errores semánticos.
    return nuevoValorResultadoVacio();
}

// Constructor para un nuevo nodo de instrucciones
AbstractExpresion* nuevoInstruccionesExpresion() {
    InstruccionesExpresion* nodo = malloc(sizeof(InstruccionesExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretInstrucciones, "Instrucciones", 0, 0);

    return (AbstractExpresion*) nodo;
}


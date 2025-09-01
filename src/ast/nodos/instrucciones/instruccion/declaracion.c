#include "declaracion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

Result interpretDeclaracionVariable(AbstractExpresion* nodo, Context* context) {
    DeclaracionVariable* self = (DeclaracionVariable*) nodo;

    // Si hay una expresión de asignación
    if (nodo->numHijos > 0) {
        Result resultado = nodo->hijos[0]->interpret(nodo->hijos[0], context);

        // Comprobación de tipos
        if (resultado.tipo != self->tipo) {
            char description[256];
            snprintf(description, sizeof(description), "Error de tipos: no se puede asignar un valor de tipo '%s' a la nueva variable '%s' de tipo '%s'.", 
                labelTipoDato[resultado.tipo], self->nombre, labelTipoDato[self->tipo]);
            
            // Se reporta el error semántico 
            add_error_to_report("Semántico", self->nombre, description, self->line, self->column);
            return nuevoValorResultadoVacio();
        }
        
        // Si los tipos son correctos, se agrega el símbolo con su valor
        agregarSymbol(context, nuevoVariable(self->nombre, resultado.valor, self->tipo), self->line, self->column);

    } else {
        // Si no hay asignación, se agrega el símbolo con valor NULL
        agregarSymbol(context, nuevoVariable(self->nombre, NULL, self->tipo), self->line, self->column);
    }
    
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoDeclaracionVariables(TipoDato tipo, char* nombre, AbstractExpresion* expresion, int line, int column) {
    DeclaracionVariable* nodo = malloc(sizeof(DeclaracionVariable));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretDeclaracionVariable, "Declaracion");
    nodo->tipo = tipo;
    nodo->nombre = nombre;
    nodo->line = line;
    nodo->column = column;

    if (expresion) agregarHijo((AbstractExpresion*) nodo, expresion);
    return (AbstractExpresion*) nodo;
}

#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "declaracion.h"

#include <stdlib.h>
#include <stdio.h>

Result interpretDeclaracionVariable(AbstractExpresion* nodo, Context* context) {
    DeclaracionVariable* self = (DeclaracionVariable*) nodo;

    if (nodo->numHijos > 0) {
        Result resultado = nodo->hijos[0]->interpret(nodo->hijos[0], context);
        if (resultado.tipo == self->tipo) {
            // Pasamos la ubicación guardada al crear el símbolo
            agregarSymbol(context, nuevoVariable(self->nombre, resultado.valor, self->tipo), self->line, self->column);
            return nuevoValorResultadoVacio();
        }
        printf("Error tipos incorrectos \n");
    }
    agregarSymbol(context, nuevoVariable(self->nombre, NULL, self->tipo), self->line, self->column);
    return nuevoValorResultadoVacio();
}

// ...
// Actualizamos la firma de la función
AbstractExpresion* nuevoDeclaracionVariables(TipoDato tipo, char* nombre, AbstractExpresion* expresion, int line, int column) {
    DeclaracionVariable* nodo = malloc(sizeof(DeclaracionVariable));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretDeclaracionVariable);
    nodo->tipo = tipo;
    nodo->nombre = nombre;
    // Guardamos la línea y columna en el nodo para usarlas después
    nodo->line = line;
    nodo->column = column;

    if (expresion) agregarHijo((AbstractExpresion*) nodo, expresion);
    return (AbstractExpresion*) nodo;
}

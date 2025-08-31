#include "reasignacion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// La función que interpreta una reasignación
Result interpretReasignacion(AbstractExpresion* self, Context* context) {
    ReasignacionExpresion* nodo = (ReasignacionExpresion*) self;

    // 1. Buscar si la variable a reasignar existe en algún ámbito visible.
    Symbol* simboloEncontrado = buscarTablaSimbolos(context, nodo->nombre);

    // 2. Comprobación Semántica #1: ¿Existe la variable?
    if (!simboloEncontrado) {
        char description[256];
        snprintf(description, sizeof(description), "Error de asignación: la variable '%s' no ha sido declarada.", nodo->nombre);
        add_error_to_report("Semántico", nodo->nombre, description, nodo->line, nodo->column);
        return nuevoValorResultadoVacio();
    }

    // 3. Si existe, interpretar la expresión de la derecha para obtener el nuevo valor.
    Result nuevoValorResult = self->hijos[0]->interpret(self->hijos[0], context);

    // 4. Comprobación Semántica #2: ¿Son compatibles los tipos?
    if (simboloEncontrado->tipo != nuevoValorResult.tipo) {
        char description[256];
        snprintf(description, sizeof(description), "Error de tipos: no se puede asignar un valor de tipo '%s' a la variable '%s' de tipo '%s'.", 
            labelTipoDato[nuevoValorResult.tipo], nodo->nombre, labelTipoDato[simboloEncontrado->tipo]);
        add_error_to_report("Semántico", nodo->nombre, description, nodo->line, nodo->column);
        return nuevoValorResultadoVacio();
    }
    
    // 5. Si todo es correcto, se actualiza el valor en la tabla de símbolos.
    // Es importante liberar la memoria del valor antiguo antes de asignar el nuevo.
    if(simboloEncontrado->valor) {
        free(simboloEncontrado->valor);
    }
    simboloEncontrado->valor = nuevoValorResult.valor;

    return nuevoValorResultadoVacio();
}

// El constructor para el nodo de reasignación
AbstractExpresion* nuevoReasignacionExpresion(char* nombre, AbstractExpresion* expresion, int line, int column) {
    ReasignacionExpresion* nodo = malloc(sizeof(ReasignacionExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretReasignacion, "Reasignacion");
    nodo->nombre = nombre;
    nodo->line = line;
    nodo->column = column;

    agregarHijo((AbstractExpresion*)nodo, expresion);
    
    return (AbstractExpresion*)nodo;
}

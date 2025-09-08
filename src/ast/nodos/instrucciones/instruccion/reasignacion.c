#include "reasignacion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// La función que interpreta una reasignación
Result interpretReasignacion(AbstractExpresion* self, Context* context) {
    ReasignacionExpresion* nodo = (ReasignacionExpresion*) self;
    Symbol* simbolo = buscarTablaSimbolos(context, nodo->nombre);

    if (!simbolo || simbolo->clase != VARIABLE) {
        // ... (Manejo de error si no es una variable declarada)
        return nuevoValorResultadoVacio();
    }

    // Si es constante, no se puede reasignar
    if (simbolo->es_constante) {
        char description[256];
        snprintf(description, sizeof(description), "No se puede reasignar la constante 'final' '%s'.", nodo->nombre);
        add_error_to_report("Semantico", nodo->nombre, description, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    Result nuevo_valor = self->hijos[0]->interpret(self->hijos[0], context);

    if (has_semantic_error_been_found()) {
        free(nuevo_valor.valor);
        return nuevoValorResultadoVacio();
    }
    
    // <<<< BLOQUE DE CÓDIGO MODIFICADO >>>>
    // Comprobar si los tipos son compatibles para la asignación.
    bool es_compatible = (simbolo->tipo == nuevo_valor.tipo);

    // Excepción: Se permite asignar un valor INT a una variable CHAR.
    if (simbolo->tipo == CHAR && nuevo_valor.tipo == INT) {
        es_compatible = true;
    }

    // Si no son compatibles, se reporta un error.
    if (!es_compatible) {
        char description[256];
        snprintf(description, sizeof(description), "Error de tipos: no se puede asignar un valor de tipo '%s' a la variable '%s' de tipo '%s'.", 
            labelTipoDato[nuevo_valor.tipo], nodo->nombre, labelTipoDato[simbolo->tipo]);
        add_error_to_report("Semántico", nodo->nombre, description, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }
    
    // <<<< CORREGIDO: Actualizamos el valor a través de la union >>>>
    if(simbolo->info.var.valor) {
        if (simbolo->tipo == ARRAY) {
            liberarArray((ArrayValue*)simbolo->info.var.valor);
        } else {
            free(simbolo->info.var.valor);
        }
    }
    simbolo->info.var.valor = nuevo_valor.valor;
    
    return nuevoValorResultadoVacio();
}

// El constructor para el nodo de reasignación
AbstractExpresion* nuevoReasignacionExpresion(char* nombre, AbstractExpresion* expresion, int line, int column) {
    ReasignacionExpresion* nodo = malloc(sizeof(ReasignacionExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretReasignacion, "Reasignacion", line, column);
    nodo->nombre = nombre;
    nodo->line = line;
    nodo->column = column;

    agregarHijo((AbstractExpresion*)nodo, expresion);
    
    return (AbstractExpresion*)nodo;
}

#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "expresiones.h"
#include "error_reporter.h"

#include <stdlib.h>
#include <stdio.h>

Result interpretExpresionLenguaje(AbstractExpresion* self, Context* context) {
    ExpresionLenguaje* nodo = (ExpresionLenguaje*) self;
    calcularResultados(nodo, context);
    
    Operacion op = (*nodo->tablaOperaciones)[nodo->izquierda.tipo][nodo->derecha.tipo];
    if (op == NULL) {
        char description[256];
        snprintf(description, sizeof(description), "La operacion '%s' no esta definida entre los tipos %s y %s.",
            self->node_type,
            labelTipoDato[nodo->izquierda.tipo], 
            labelTipoDato[nodo->derecha.tipo]
        );
        add_error_to_report("Semantico", self->node_type, description, self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }
    return op(nodo);
}

Result interpretUnarioLenguaje(AbstractExpresion* self, Context* context) {
    ExpresionLenguaje* nodo = (ExpresionLenguaje*) self;
    calcularResultadoIzquierdo(nodo, context);

    Operacion op = (*nodo->tablaOperaciones)[nodo->izquierda.tipo][NULO];
    if (op == NULL) {
        char description[256];
        snprintf(description, sizeof(description), "La operacion unaria '%s' no esta definida para el tipo %s.",
            self->node_type,
            labelTipoDato[nodo->izquierda.tipo]
        );
        add_error_to_report("Semantico", self->node_type, description, self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }
    return op(nodo);
}

ExpresionLenguaje* nuevoExpresionLenguaje(Interpret funcionEspecifica, AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column) {
    // reservar el espacio en memoria y obtener el puntero a este
    ExpresionLenguaje* nodo = malloc(sizeof(ExpresionLenguaje));
    if (!nodo) return NULL;

    //asignar valores
    buildAbstractExpresion(&nodo->base, funcionEspecifica, "ExpresionLenguaje", line, column);

    //guardar los punteros en lo hijos de este arbol a los lados de la expresion
    if (izquierda) agregarHijo((AbstractExpresion*) nodo, izquierda);
    if (derecha) agregarHijo((AbstractExpresion*) nodo, derecha);
    
    return nodo;
}

void calcularResultadoIzquierdo(ExpresionLenguaje* self, Context* context) {
    self->izquierda = self->base.hijos[0]->interpret(self->base.hijos[0], context);
}

void calcularResultadoDerecho(ExpresionLenguaje* self, Context* context) {
    self->derecha = self->base.hijos[1]->interpret(self->base.hijos[1], context);
}

void calcularResultados(ExpresionLenguaje* self, Context* context) {
    calcularResultadoIzquierdo(self, context);
    calcularResultadoDerecho(self, context);
}
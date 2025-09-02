#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "expresiones.h"
#include "error_reporter.h"

#include <stdlib.h>
#include <stdio.h>

// en src/ast/nodos/expresiones/expresiones.c

Result interpretExpresionLenguaje(AbstractExpresion* self, Context* context) {
    ExpresionLenguaje* nodo = (ExpresionLenguaje*)self;

    Result izquierda = self->hijos[0]->interpret(self->hijos[0], context);
    Result derecha = self->hijos[1]->interpret(self->hijos[1], context);

    if (has_semantic_error_been_found()) {
        free(izquierda.valor); free(derecha.valor);
        return nuevoValorResultadoVacio();
    }
    
    // Almacenamos los resultados temporalmente en el nodo para pasarlos a la función de operación
    nodo->izquierda = izquierda;
    nodo->derecha = derecha;

    Operacion op = (*nodo->tablaOperaciones)[izquierda.tipo][derecha.tipo];
    if (op == NULL) {
        char desc[256];
        snprintf(desc, sizeof(desc), "La operacion '%s' no esta definida entre los tipos %s y %s.", self->node_type, labelTipoDato[izquierda.tipo], labelTipoDato[derecha.tipo]);
        add_error_to_report("Semantico", self->node_type, desc, self->line, self->column, context->nombre_completo);
        free(izquierda.valor); free(derecha.valor);
        return nuevoValorResultadoVacio();
    }
    
    Result resultado_final = op(nodo);
    
    free(izquierda.valor);
    free(derecha.valor);
    
    return resultado_final;
}


ExpresionLenguaje *nuevoExpresionLenguaje(Interpret funcionEspecifica, AbstractExpresion *izquierda, AbstractExpresion *derecha, int line, int column)
{
    ExpresionLenguaje *nodo = malloc(sizeof(ExpresionLenguaje));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, funcionEspecifica, "ExpresionLenguaje", line, column);

    if (izquierda) agregarHijo((AbstractExpresion *)nodo, izquierda);
    if (derecha) agregarHijo((AbstractExpresion *)nodo, derecha);

    return nodo;
}

void calcularResultadoIzquierdo(ExpresionLenguaje *self, Context *context)
{
    self->izquierda = self->base.hijos[0]->interpret(self->base.hijos[0], context);
}

void calcularResultadoDerecho(ExpresionLenguaje *self, Context *context)
{
    self->derecha = self->base.hijos[1]->interpret(self->base.hijos[1], context);
}

void calcularResultados(ExpresionLenguaje *self, Context *context)
{
    calcularResultadoIzquierdo(self, context);
    calcularResultadoDerecho(self, context);
}
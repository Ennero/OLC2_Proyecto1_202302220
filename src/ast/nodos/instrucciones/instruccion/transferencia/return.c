// src/.../transferencia/return.c
#include "ast/nodos/builders.h"
#include <stdlib.h>

Result interpretReturnExpresion(AbstractExpresion* self, Context* context) {
    Result valor_a_retornar;

    // Comprobar si hay una expresión de retorno (return valor;)
    if (self->numHijos > 0) {
        valor_a_retornar = self->hijos[0]->interpret(self->hijos[0], context);
    } else {
        // Si no, es un return vacío (return;)
        valor_a_retornar = nuevoValorResultadoVacio();
    }

    // Para poder propagar el valor de retorno COMPLETO (valor y tipo),
    // lo "envolvemos" en un puntero.
    Result* resultado_envuelto = malloc(sizeof(Result));
    *resultado_envuelto = valor_a_retornar;

    // Devolvemos la señal RETURN_T, llevando el resultado envuelto como carga.
    return nuevoValorResultado(resultado_envuelto, RETURN_T);
}

AbstractExpresion* nuevoReturnExpresion(AbstractExpresion* expresion, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretReturnExpresion, "ReturnStatement", line, column);
    // La expresión es opcional (para return;)
    if (expresion) {
        agregarHijo(nodo, expresion);
    }
    return nodo;
}
#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "expresiones.h"
#include <stdio.h>
#include <stdlib.h>

Result interpretExpresionLenguaje(AbstractExpresion* self, Context* context) {
    ExpresionLenguaje* nodo = (ExpresionLenguaje*) self;
    
    Result resultado1 = nodo->base.hijos[0]->interpret(nodo->base.hijos[0], context);
    Result resultado2 = nodo->base.hijos[1]->interpret(nodo->base.hijos[1], context);
    
    switch (nodo->op) {
        case '+':
            if (resultado1.tipo == 'I' && resultado2.tipo == 'I') {
                // MODIFICADO: Asignar memoria para el resultado final
                int* valorFinal = malloc(sizeof(int));
                *valorFinal = *((int*) resultado1.valor) + *((int*) resultado2.valor);
                
                // Liberar la memoria de los operandos que ya no se usan
                free(resultado1.valor);
                free(resultado2.valor);
                
                return nuevoValorResultado((void*) valorFinal, 'I');
            }
            break; // No olvides el break
        case '-':
            if (resultado1.tipo == 'I' && resultado2.tipo == 'I') {
                // NUEVO: Lógica para la resta
                int* valorFinal = malloc(sizeof(int));
                *valorFinal = *((int*) resultado1.valor) - *((int*) resultado2.valor);
                
                free(resultado1.valor);
                free(resultado2.valor);
                
                return nuevoValorResultado((void*) valorFinal, 'I');
            }
            break;
        default:
            fprintf(stderr, "Operador desconocido %c\n", nodo->op);
            // Liberar memoria si no se pudo operar
            free(resultado1.valor);
            free(resultado2.valor);
            return nuevoValorResultadoVacio();
    }
    // Si los tipos no coinciden, liberar y retornar vacío
    free(resultado1.valor);
    free(resultado2.valor);
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoExpresionLenguaje(char op, AbstractExpresion* izquierda, AbstractExpresion* derecha) {
    ExpresionLenguaje* nodo = malloc(sizeof(ExpresionLenguaje));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretExpresionLenguaje);
    nodo->op = op;
    if (izquierda) agregarHijo((AbstractExpresion*) nodo, izquierda);
    if (derecha) agregarHijo((AbstractExpresion*) nodo, derecha);
    return (AbstractExpresion*) nodo;
}
// en: src/ast/nodos/instrucciones/instruccion/ciclo/for.h

#ifndef FOR_H
#define FOR_H

#include "ast/AbstractExpresion.h"

// La estructura es simple, solo la base. Las 4 partes del 'for'
// (init, condition, update, body) serán sus hijos.
typedef struct {
    AbstractExpresion base;
} ForExpresion;

// Prototipo del constructor que usará el parser
AbstractExpresion* nuevoForExpresion(
    AbstractExpresion* init, 
    AbstractExpresion* cond, 
    AbstractExpresion* update, 
    AbstractExpresion* bloque, 
    int line, 
    int column
);

#endif //FOR_H
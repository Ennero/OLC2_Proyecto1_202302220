// en: src/ast/nodos/instrucciones/instruccion/ciclo/while.h

#ifndef WHILE_H
#define WHILE_H

#include "ast/AbstractExpresion.h"

// La estructura es simple, solo contiene la base.
// La condición y el bloque se almacenarán como hijos.
typedef struct {
    AbstractExpresion base;
} WhileExpresion;

// Prototipo del constructor que usará el parser
AbstractExpresion* nuevoWhileExpresion(AbstractExpresion* condicion, AbstractExpresion* bloque, int line, int column);

#endif //WHILE_H
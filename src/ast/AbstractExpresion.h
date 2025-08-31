#ifndef ABSTRACT_EXPRESION_H
#define ABSTRACT_EXPRESION_H

#include "context/result.h"
#include "context/context.h"

#include <stddef.h>

typedef struct AbstractExpresion AbstractExpresion;
typedef Result (*Interpret)(AbstractExpresion*, Context*);

struct AbstractExpresion {
    Interpret interpret;
    const char* node_type; // <<<< NUEVO: Un nombre para identificar el tipo de nodo
    AbstractExpresion** hijos;
    size_t numHijos;
};

void agregarHijo(AbstractExpresion* padre, AbstractExpresion* hijo);
void liberarAST(AbstractExpresion* raiz);
// La firma ahora acepta el nombre del tipo de nodo
void buildAbstractExpresion(AbstractExpresion* base, Interpret interpretPuntero, const char* node_type);

#endif

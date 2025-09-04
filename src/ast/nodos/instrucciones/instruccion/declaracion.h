#ifndef DECLARACION_H // <-- CORREGIDO
#define DECLARACION_H // <-- CORREGIDO

#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "context/result.h"

typedef struct {
    AbstractExpresion base;
    char* nombre;
    TipoDato tipo;
    int line;
    int column;
} DeclaracionVariable;

Result interpretDeclaracionVariable(AbstractExpresion*, Context*);

#endif // DECLARACION_H <-- CORREGIDO
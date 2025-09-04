#ifndef PRIMITIVOS_H // <-- CORREGIDO
#define PRIMITIVOS_H // <-- CORREGIDO

#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "context/result.h"

typedef struct {
    AbstractExpresion base;
    TipoDato tipo;
    char* valor;
} PrimitivoExpresion;

Result interpretPrimitivoExpresion(AbstractExpresion*, Context*);
#endif // PRIMITIVOS_H <-- CORREGIDO
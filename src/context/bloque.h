#ifndef BLOQUE_H
#define BLOQUE_H

#include "ast/AbstractExpresion.h"

// No necesita campos adicionales, solo la base.
typedef struct {
    AbstractExpresion base;
} BloqueExpresion;

#endif

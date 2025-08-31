#ifndef IDENTIFICADORES_H
#define IDENTIFICADORES_H

#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "context/result.h"

// <<<< CAMBIO: Se añaden campos para la ubicación >>>>
// Esta estructura ahora almacena la línea y columna donde se encontró
// el identificador en el código fuente. Esto es crucial para poder
// reportar errores semánticos con la ubicación precisa.
typedef struct {
    AbstractExpresion base;
    char* nombre;
    int line;     // Línea donde se encontró el identificador
    int column;   // Columna donde se encontró el identificador
} IdentificadorExpresion;

Result interpretIdentificadorExpresion(AbstractExpresion*, Context*);

#endif


#ifndef REASIGNACION_H
#define REASIGNACION_H

#include "ast/AbstractExpresion.h"

// Estructura para un nodo de reasignación.
// Contiene el nombre de la variable y su ubicación para errores.
// El nuevo valor a asignar será el único hijo de este nodo.
typedef struct {
    AbstractExpresion base;
    char* nombre;
    int line;
    int column;
} ReasignacionExpresion;

#endif

#ifndef ASIGNACION_COMPUESTA_H
#define ASIGNACION_COMPUESTA_H

#include "ast/AbstractExpresion.h"

// Estructura para un nodo de asignación compuesta (ej. +=, -=)
typedef struct {
    AbstractExpresion base;
    char* nombre;
    char op_type; // Almacena el carácter del operador: '+', '-', '*', etc.
    int line;
    int column;
} AsignacionCompuestaExpresion;

#endif // ASIGNACION_COMPUESTA_H
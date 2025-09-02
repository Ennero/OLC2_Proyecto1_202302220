#ifndef IF_H
#define IF_H

#include "ast/AbstractExpresion.h"

// La estructura para el nodo IF es simple.
// No necesita campos adicionales porque los hijos se guardan en el arreglo base.
// Convención de hijos:
// - hijos[0]: La condición (expr)
// - hijos[1]: El bloque a ejecutar si la condición es verdadera (bloque)
// - hijos[2]: El bloque "else" o el siguiente "else if" (bloque o if_stmt). Puede ser NULL.
typedef struct {
    AbstractExpresion base;
} IfExpresion;

#endif //IF_H
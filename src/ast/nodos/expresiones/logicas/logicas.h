#ifndef LOGICAS_H
#define LOGICAS_H

#include "ast/nodos/expresiones/expresiones.h"

/* --- Operador Lógico AND (&&) --- */

// Función que implementa la lógica 'booleano && booleano'
Result logicoAndBooleano(ExpresionLenguaje* self);
// Tabla de operaciones para AND. Solo tendrá una entrada válida.
extern Operacion tablaOperacionesAnd[TIPO_COUNT][TIPO_COUNT];


/* --- Operador Lógico OR (||) --- */

// Función que implementa la lógica 'booleano || booleano'
Result logicoOrBooleano(ExpresionLenguaje* self);
// Tabla de operaciones para OR.
extern Operacion tablaOperacionesOr[TIPO_COUNT][TIPO_COUNT];


/* --- Operador Lógico NOT (!) --- */

// Función que implementa la lógica '!booleano'
Result logicoNotBooleano(Result res);
// Tabla de operaciones para el NOT unario.
extern UnaryOperacion tablaOperacionesNot[TIPO_COUNT];

#endif
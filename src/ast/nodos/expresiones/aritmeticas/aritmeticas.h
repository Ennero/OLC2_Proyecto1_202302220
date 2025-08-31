#ifndef ARITMETICAS_H
#define ARITMETICAS_H

#include "ast/nodos/expresiones/expresiones.h"

/* SUMA */
extern Operacion tablaOperacionesSuma[TIPO_COUNT][TIPO_COUNT];
/* RESTA */
extern Operacion tablaOperacionesResta[TIPO_COUNT][TIPO_COUNT];
/* UNARIO */
extern Operacion tablaOperacionesUnario[TIPO_COUNT][TIPO_COUNT];
/* MULTIPLICACIÓN */
extern Operacion tablaOperacionesMultiplicacion[TIPO_COUNT][TIPO_COUNT];
/* DIVISIÓN */
extern Operacion tablaOperacionesDivision[TIPO_COUNT][TIPO_COUNT];
/* MÓDULO */
extern Operacion tablaOperacionesModulo[TIPO_COUNT][TIPO_COUNT];

#endif

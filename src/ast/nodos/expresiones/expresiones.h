#ifndef EXPRESIONES_H
#define EXPRESIONES_H

#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "context/result.h"

typedef struct ExpresionLenguaje ExpresionLenguaje;

// Typedef para operaciones BINARIAS (ej. a + b)
typedef Result (*Operacion)(ExpresionLenguaje*);

// <<<< NUEVO: Typedef para operaciones UNARIAS (ej. -a) >>>>
// Nota que recibe un 'Result', no un 'ExpresionLenguaje*', porque no
// necesita acceder a los operandos izquierdo/derecho.
typedef Result (*UnaryOperacion)(Result);

struct ExpresionLenguaje{
    AbstractExpresion base;
    Result izquierda;
    Result derecha;
    Operacion (*tablaOperaciones)[TIPO_COUNT][TIPO_COUNT];
};


//interpret basico de expresiones
Result interpretExpresionLenguaje(AbstractExpresion* self, Context* context);
Result interpretUnarioLenguaje(AbstractExpresion* self, Context* context);
ExpresionLenguaje* nuevoExpresionLenguaje(Interpret funcionEspecifica, AbstractExpresion* izquierda, AbstractExpresion* derecha);
void calcularResultadoIzquierdo(ExpresionLenguaje* self, Context* context);
void calcularResultadoDerecho(ExpresionLenguaje* self, Context* context);
void calcularResultados(ExpresionLenguaje* self, Context* context);

#endif
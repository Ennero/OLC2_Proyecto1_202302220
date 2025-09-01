#ifndef RESULT_H
#define RESULT_H

typedef enum {
    BOOLEAN,
    CHAR,
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    NULO,
    TIPO_COUNT
} TipoDato;

extern char* labelTipoDato[];

// Estructura para el resultado de una expresión
typedef struct {
    TipoDato tipo;
    void* valor;
} Result;

// Funciones para manejar resultados
TipoDato tipoResultante(Result, Result);
Result nuevoValorResultado(void* valor, TipoDato tipo);
Result nuevoValorResultadoVacio(void);

#endif
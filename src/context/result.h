#ifndef RESULT_H
#define RESULT_H

typedef enum {
    BOOLEAN,
    CHAR,
    INT,
    FLOAT,      // <<<< SE MANTIENE
    DOUBLE,     // <<<< SE MANTIENE
    STRING,
    NULO,
    TIPO_COUNT
} TipoDato;

extern char* labelTipoDato[];
// ... (resto del archivo sin cambios)
typedef struct {
    TipoDato tipo;
    void* valor;
} Result;

TipoDato tipoResultante(Result, Result);
Result nuevoValorResultado(void* valor, TipoDato tipo);
Result nuevoValorResultadoVacio(void);

#endif
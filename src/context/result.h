#ifndef RESULT_H
#define RESULT_H

// Definición de tipos de datos
typedef enum
{
    BOOLEAN,
    CHAR,
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    ARRAY,
    BREAK_T,
    CONTINUE_T,
    RETURN_T,
    NULO,
    TIPO_COUNT
} TipoDato;

// Etiquetas legibles para los tipos de datos
extern char *labelTipoDato[];

// Estructura para el resultado de una expresión
typedef struct
{
    TipoDato tipo;
    void *valor;
} Result;

// Funciones para manejar resultados
TipoDato tipoResultante(Result, Result);
Result nuevoValorResultado(void *valor, TipoDato tipo);
Result nuevoValorResultadoVacio(void);

// Utilidades de conversión (widening casting implícito)
// Retorna 1 si es permitido convertir implícitamente 'from' -> 'to' sin pérdida
int can_widen(TipoDato from, TipoDato to);

// Convierte (promueve) un Result primitivo a 'to'. Asume can_widen(from,to) o tipos iguales.
// Libera el valor anterior de 'src' si se crea uno nuevo. Devuelve un Result con el nuevo tipo.
Result widen_to(Result src, TipoDato to);

#endif
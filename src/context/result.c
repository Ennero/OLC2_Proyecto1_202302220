#include "result.h"
#include <stddef.h>
#include <stdio.h>

// Etiquetas para los tipos de datos
char* labelTipoDato[TIPO_COUNT] = {
    [BOOLEAN]   = "boolean",
    [CHAR]      = "char",
    [INT]       = "int",
    [FLOAT]     = "float",      
    [DOUBLE]    = "double",
    [STRING]    = "string",
    [ARRAY]     = "array",
    [BREAK_T]   = "<break>",
    [CONTINUE_T]= "<continue>",
    [RETURN_T]  = "<return>",
    [NULO]      = "null"
};

// Crear un nuevo valor de resultado
Result nuevoValorResultado(void* valor, TipoDato tipo) {
    Result resultado;
    resultado.tipo = tipo;
    resultado.valor = valor;
    return resultado;
}

// Crear un nuevo valor de resultado vacío
Result nuevoValorResultadoVacio() {
    Result resultado;
    resultado.tipo = NULO;
    resultado.valor = NULL;
    return resultado;
}

// Determinar el tipo resultante de dos valores
TipoDato tipoResultante(Result valor1, Result valor2) {
    if (valor1.tipo >= valor2.tipo) {
        return valor1.tipo;
    } else {
        return valor2.tipo;
    }
}
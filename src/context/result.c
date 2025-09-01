#include "result.h"
#include <stddef.h>
#include <stdio.h>

char* labelTipoDato[TIPO_COUNT] = {
    [BOOLEAN] = "boolean",
    [CHAR] = "char",
    [INT] = "int",
    [FLOAT] = "float",      // <<<< SE MANTIENE
    [DOUBLE] = "double",
    [STRING] = "string",
    [NULO] = "null"
};
// ... (resto del archivo sin cambios)
Result nuevoValorResultado(void* valor, TipoDato tipo) {
    Result resultado;
    resultado.tipo = tipo;
    resultado.valor = valor;
    return resultado;
}

Result nuevoValorResultadoVacio() {
    Result resultado;
    resultado.tipo = NULO;
    resultado.valor = NULL;
    return resultado;
}

TipoDato tipoResultante(Result valor1, Result valor2) {
    if (valor1.tipo >= valor2.tipo) {
        return valor1.tipo;
    } else {
        return valor2.tipo;
    }
}
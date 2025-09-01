#include "aritmeticas.h"
#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "context/result.h"
#include <stdlib.h>

// --- Implementaciones ---
Result sumarIntInt(ExpresionLenguaje* self) {
    int* res = malloc(sizeof(int));
    *res =  *((int*)self->izquierda.valor) + *((int*)self->derecha.valor);
    return nuevoValorResultado(res, INT);
}

Result sumarFloatFloat(ExpresionLenguaje* self) {
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) + *((float*)self->derecha.valor);
    return nuevoValorResultado(res, FLOAT);
}

Result sumarDoubleDouble(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) + *((double*)self->derecha.valor);
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarIntFloat(ExpresionLenguaje* self) {
    float* res = malloc(sizeof(float));
    *res = (float)(*((int*)self->izquierda.valor)) + *((float*)self->derecha.valor);
    return nuevoValorResultado(res, FLOAT);
}

Result sumarFloatInt(ExpresionLenguaje* self) {
    float* res = malloc(sizeof(float));
    *res = *((float*)self->izquierda.valor) + (float)(*((int*)self->derecha.valor));
    return nuevoValorResultado(res, FLOAT);
}

Result sumarIntDouble(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = (double)(*((int*)self->izquierda.valor)) + *((double*)self->derecha.valor);
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarDoubleInt(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) + (double)(*((int*)self->derecha.valor));
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarFloatDouble(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = (double)(*((float*)self->izquierda.valor)) + *((double*)self->derecha.valor);
    return nuevoValorResultado(res, DOUBLE);
}

Result sumarDoubleFloat(ExpresionLenguaje* self) {
    double* res = malloc(sizeof(double));
    *res = *((double*)self->izquierda.valor) + (double)(*((float*)self->derecha.valor));
    return nuevoValorResultado(res, DOUBLE);
}



// --- FUNCIONES DE CONCATENACIÓN ---
Result concatenarStringString(ExpresionLenguaje* self) {
    char* s1 = (char*)self->izquierda.valor;
    char* s2 = (char*)self->derecha.valor;
    char* res = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(res, s1);
    strcat(res, s2);
    return nuevoValorResultado(res, STRING);
}


// Convierte el operando derecho a string y luego concatena
Result concatenarString_X(ExpresionLenguaje* self, const char* formato) {
    char* s1 = (char*)self->izquierda.valor;
    char s2_buffer[64]; // Buffer para el número o booleano convertido

    switch(self->derecha.tipo) {
        case INT:    sprintf(s2_buffer, formato, *(int*)self->derecha.valor); break;
        case CHAR:   sprintf(s2_buffer, formato, (char)(*(int*)self->derecha.valor)); break;
        case FLOAT:  sprintf(s2_buffer, formato, *(float*)self->derecha.valor); break;
        case DOUBLE: sprintf(s2_buffer, formato, *(double*)self->derecha.valor); break;
        case BOOLEAN: sprintf(s2_buffer, "%s", (*(int*)self->derecha.valor ? "true" : "false")); break;
        default: return nuevoValorResultadoVacio(); // No debería ocurrir
    }

    char* res = malloc(strlen(s1) + strlen(s2_buffer) + 1);
    strcpy(res, s1);
    strcat(res, s2_buffer);
    return nuevoValorResultado(res, STRING);
}

// Convierte el operando izquierdo a string y luego concatena
Result concatenar_X_String(ExpresionLenguaje* self, const char* formato) {
    char* s2 = (char*)self->derecha.valor;
    char s1_buffer[64];

    switch(self->izquierda.tipo) {
        case INT:     sprintf(s1_buffer, formato, *(int*)self->izquierda.valor); break;
        case CHAR:    sprintf(s1_buffer, formato, (char)(*(int*)self->izquierda.valor)); break;
        case FLOAT:   sprintf(s1_buffer, formato, *(float*)self->izquierda.valor); break;
        case DOUBLE:  sprintf(s1_buffer, formato, *(double*)self->izquierda.valor); break;
        case BOOLEAN: sprintf(s1_buffer, "%s", (*(int*)self->izquierda.valor ? "true" : "false")); break;
        default: return nuevoValorResultadoVacio();
    }
    
    char* res = malloc(strlen(s1_buffer) + strlen(s2) + 1);
    strcpy(res, s1_buffer);
    strcat(res, s2);
    return nuevoValorResultado(res, STRING);
}


// Wrappers para llamar a las funciones genéricas con el formato correcto
Result concatenarStringInt(ExpresionLenguaje* self) { return concatenarString_X(self, "%d"); }
Result concatenarStringChar(ExpresionLenguaje* self) { return concatenarString_X(self, "%c"); }
Result concatenarStringDouble(ExpresionLenguaje* self) { return concatenarString_X(self, "%g"); }
Result concatenarStringBoolean(ExpresionLenguaje* self) { return concatenarString_X(self, NULL); }

Result concatenarIntString(ExpresionLenguaje* self) { return concatenar_X_String(self, "%d"); }
Result concatenarCharString(ExpresionLenguaje* self) { return concatenar_X_String(self, "%c"); }
Result concatenarDoubleString(ExpresionLenguaje* self) { return concatenar_X_String(self, "%g"); }
Result concatenarBooleanString(ExpresionLenguaje* self) { return concatenar_X_String(self, NULL); }



// --- TABLA DE OPERACIONES DE SUMA COMPLETAMENTE CORREGIDA ---
Operacion tablaOperacionesSuma[TIPO_COUNT][TIPO_COUNT] = {
    // Columna:   BOOLEAN,                    CHAR,                  INT,                   FLOAT,                     DOUBLE,                    STRING
    /* Fila */
    [BOOLEAN] = { [STRING] = concatenarBooleanString },
    [CHAR]    = { [CHAR] = sumarIntInt,      [INT] = sumarIntInt,     [FLOAT] = sumarFloatInt, [DOUBLE] = sumarDoubleInt,  [STRING] = concatenarCharString },
    [INT]     = { [CHAR] = sumarIntInt,      [INT] = sumarIntInt,     [FLOAT] = sumarIntFloat, [DOUBLE] = sumarIntDouble,  [STRING] = concatenarIntString },
    [FLOAT]   = { [CHAR] = sumarFloatInt,    [INT] = sumarFloatInt,   [FLOAT] = sumarFloatFloat,[DOUBLE] = sumarFloatDouble,[STRING] = concatenarDoubleString },
    [DOUBLE]  = { [CHAR] = sumarDoubleInt,   [INT] = sumarDoubleInt,  [FLOAT] = sumarDoubleFloat,[DOUBLE]= sumarDoubleDouble,[STRING] = concatenarDoubleString },
    [STRING]  = { [BOOLEAN] = concatenarStringBoolean, [CHAR] = concatenarStringChar, [INT] = concatenarStringInt, [FLOAT] = concatenarStringDouble, [DOUBLE] = concatenarStringDouble, [STRING] = concatenarStringString }
};


AbstractExpresion* nuevoSumaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column) {
    ExpresionLenguaje* sumaExpresion = nuevoExpresionLenguaje(interpretExpresionLenguaje, izquierda, derecha, line, column);
    sumaExpresion->base.node_type = "Suma";
    sumaExpresion->tablaOperaciones = &tablaOperacionesSuma;
    return (AbstractExpresion*) sumaExpresion;
}
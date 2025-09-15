#include "primitivos.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Función auxiliar para convertir secuencias de escape Unicode \uXXXX a su valor entero
static int parse_unicode_escape(const char *escape_sequence)
{
    char hex[5];
    strncpy(hex, escape_sequence, 4);
    hex[4] = '\0';
    return (int)strtol(hex, NULL, 16);
}

// Función auxiliar para procesar las secuencias de escape en cadenas
static char *process_string_escapes(const char *input)
{
    // Primero, contamos el tamaño necesario para la cadena procesada
    size_t len = strlen(input);
    char *output = malloc(len + 1);
    if (!output)
        return NULL;

    size_t i = 0, j = 0;

    // Procesar cada carácter
    while (i < len)
    {
        if (input[i] == '\\' && i + 1 < len)
        {
            i++; 
            switch (input[i])
            {
            case 'n':
                output[j++] = '\n';
                break;
            case 't':
                output[j++] = '\t';
                break;
            case 'r':
                output[j++] = '\r';
                break;
            case '\\':
                output[j++] = '\\';
                break;
            case '\"':
                output[j++] = '\"';
                break;
            case '\'':
                output[j++] = '\'';
                break;
            case 'u':
                if (i + 4 < len)
                {
                    int unicode_val = parse_unicode_escape(&input[i + 1]);
                    output[j++] = (char)unicode_val;
                    i += 4;
                }
                break;
            default:
                output[j++] = '\\';
                output[j++] = input[i];
                break;
            }
        }
        else
        {
            output[j++] = input[i];
        }
        i++;
    }
    output[j] = '\0';
    return output;
}

// Funcion de interpretación para el nodo Primitivo
Result interpretPrimitivoExpresion(AbstractExpresion *self, Context *context)
{
    (void)context;
    PrimitivoExpresion *nodo = (PrimitivoExpresion *)self;

    // Dependiendo del tipo, convertimos el valor almacenado en cadena al tipo adecuado
    switch (nodo->tipo)
    {
    case NULO:
    {
        return nuevoValorResultado(NULL, NULO);
    }
    case INT:
    {
        int *valor = malloc(sizeof(int));
        if (strncmp(nodo->valor, "0x", 2) == 0 || strncmp(nodo->valor, "0X", 2) == 0)
        {
            *valor = (int)strtol(nodo->valor, NULL, 16);
        }
        else
        {
            *valor = (int)strtol(nodo->valor, NULL, 10);
        }
        return nuevoValorResultado(valor, INT);
    }
    case FLOAT:
    {
        float *v = malloc(sizeof(float));
        *v = atof(nodo->valor);
        return nuevoValorResultado(v, FLOAT);
    }
    case DOUBLE:
    {
        double *v = malloc(sizeof(double));
        *v = atof(nodo->valor);
        return nuevoValorResultado(v, DOUBLE);
    }
    case BOOLEAN:
    {
        int *v = malloc(sizeof(int));
        *v = strcmp(nodo->valor, "true") == 0;
        return nuevoValorResultado(v, BOOLEAN);
    }
    case CHAR:
    {
        int *valor = malloc(sizeof(int));
        char *processed = process_string_escapes(nodo->valor);
        *valor = (int)processed[0];
        free(processed);
        return nuevoValorResultado(valor, CHAR);
    }
    case STRING:
    {
        return nuevoValorResultado(process_string_escapes(nodo->valor), STRING);
    }
    default:
        return nuevoValorResultadoVacio();
    }
}

// Constructor del nodo Primitivo
AbstractExpresion *nuevoPrimitivoExpresion(char *v, TipoDato tipo, int line, int column)
{
    PrimitivoExpresion *nodo = malloc(sizeof(PrimitivoExpresion));
    if (!nodo)
        return NULL;
    buildAbstractExpresion(&nodo->base, interpretPrimitivoExpresion, "Primitivo", line, column);
    nodo->valor = v;
    nodo->tipo = tipo;
    return (AbstractExpresion *)nodo;
}
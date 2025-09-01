#include "primitivos.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// NUEVO: Función auxiliar para convertir una subcadena hexadecimal de 4 dígitos a un int.
static int parse_unicode_escape(const char *escape_sequence)
{
    char hex[5];
    strncpy(hex, escape_sequence, 4);
    hex[4] = '\0';
    return (int)strtol(hex, NULL, 16);
}

// MODIFICADO: Ahora procesa también los escapes unicode \uXXXX.
static char *process_string_escapes(const char *input)
{
    size_t len = strlen(input);
    char *output = malloc(len + 1);
    if (!output)
        return NULL;

    size_t i = 0, j = 0;
    while (i < len)
    {
        if (input[i] == '\\' && i + 1 < len)
        {
            i++; // Saltar la barra invertida
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
            // NUEVO: Manejo de secuencias de escape Unicode
            case 'u':
                if (i + 4 < len)
                {
                    int unicode_val = parse_unicode_escape(&input[i + 1]);
                    // Ojo: Esto asume que el valor unicode cabe en un char.
                    // Para un intérprete más avanzado se necesitaría manejo de UTF-8.
                    output[j++] = (char)unicode_val;
                    i += 4; // Avanzamos el índice para saltar los 4 dígitos hexadecimales.
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

// MODIFICADO: La función de interpretación ahora maneja correctamente Hex y Unicode.
Result interpretPrimitivoExpresion(AbstractExpresion *self, Context *context)
{
    (void)context;
    PrimitivoExpresion *nodo = (PrimitivoExpresion *)self;

    switch (nodo->tipo)
    {
    case INT:
    {
        int *valor = malloc(sizeof(int));
        // MODIFICADO: Usamos strtol con base 0 para detectar automáticamente "0x".
        // strtol es más robusto que atoi.
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
        float *valor = malloc(sizeof(float));
        *valor = atof(nodo->valor);
        return nuevoValorResultado(valor, FLOAT);
    }
    case DOUBLE:
    {
        double *valor = malloc(sizeof(double));
        *valor = atof(nodo->valor);
        return nuevoValorResultado(valor, DOUBLE);
    }
    case BOOLEAN:
    {
        int *valor = malloc(sizeof(int));
        *valor = strcmp(nodo->valor, "true") == 0;
        return nuevoValorResultado(valor, BOOLEAN);
    }
    case CHAR:
    {
        int *valor = malloc(sizeof(int));
        char *processed_char_content = process_string_escapes(nodo->valor);
        *valor = (int)processed_char_content[0];
        free(processed_char_content);
        return nuevoValorResultado(valor, CHAR);
    }
    case STRING:
    {
        char *processed_string = process_string_escapes(nodo->valor);
        return nuevoValorResultado(processed_string, STRING);
    }
    default:
        return nuevoValorResultadoVacio();
    }
}

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
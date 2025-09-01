#include "primitivos.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// <<<< NUEVA FUNCIÓN AUXILIAR PARA PROCESAR ESCAPES EN STRINGS >>>>
static char *process_string_escapes(const char *input)
{
    size_t len = strlen(input);
    char *output = malloc(len + 1); // La longitud final será <= que la original
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
            default: // Si no es un escape conocido, se trata como literal
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

Result interpretPrimitivoExpresion(AbstractExpresion *self, Context *context)
{
    (void)context;
    PrimitivoExpresion *nodo = (PrimitivoExpresion *)self;

    switch (nodo->tipo)
    {
    case INT:
    {
        int *valor = malloc(sizeof(int));
        *valor = atoi(nodo->valor);
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
        // Ahora guardamos el char como un entero para facilitar las operaciones.
        int *valor = malloc(sizeof(int));

        char *processed_char_content = process_string_escapes(nodo->valor);
        *valor = (int)processed_char_content[0]; // Guardamos su valor numérico.
        free(processed_char_content);

        return nuevoValorResultado(valor, CHAR);
    }
    case STRING:
    {
        // El lexer nos da el string sin comillas, solo procesamos escapes
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
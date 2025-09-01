#include "primitivos.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// <<<< NUEVA FUNCIÓN AUXILIAR PARA PROCESAR ESCAPES EN STRINGS >>>>
static char* process_string_escapes(const char* input) {
    size_t len = strlen(input);
    char* output = malloc(len + 1); // La longitud final será <= que la original
    if (!output) return NULL;

    size_t i = 0, j = 0;
    while (i < len) {
        if (input[i] == '\\' && i + 1 < len) {
            i++; // Saltar la barra invertida
            switch (input[i]) {
                case 'n': output[j++] = '\n'; break;
                case 't': output[j++] = '\t'; break;
                case 'r': output[j++] = '\r'; break;
                case '\\': output[j++] = '\\'; break;
                case '\"': output[j++] = '\"'; break;
                case '\'': output[j++] = '\''; break;
                default: // Si no es un escape conocido, se trata como literal
                    output[j++] = '\\';
                    output[j++] = input[i];
                    break;
            }
        } else {
            output[j++] = input[i];
        }
        i++;
    }
    output[j] = '\0';
    return output;
}


Result interpretPrimitivoExpresion(AbstractExpresion* self, Context* context) {
    (void)context;
    PrimitivoExpresion* nodo = (PrimitivoExpresion*) self;
    
    switch (nodo->tipo) {
        case INT: {
            int* valor_int = malloc(sizeof(int));
            if (strncmp(nodo->valor, "0x", 2) == 0 || strncmp(nodo->valor, "0X", 2) == 0) {
                *valor_int = (int)strtol(nodo->valor, NULL, 16);
            } else {
                *valor_int = atoi(nodo->valor);
            }
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_int, nodo->tipo);
        }
        case FLOAT: { 
            float* valor_float = malloc(sizeof(float));
            *valor_float = atof(nodo->valor); // atof convierte a double, pero el cast a float es seguro
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_float, nodo->tipo);
        }
        case DOUBLE: {
            double* valor_double = malloc(sizeof(double));
            *valor_double = atof(nodo->valor);
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_double, nodo->tipo);
        }
        case STRING: {
            size_t len = strlen(nodo->valor);
            char* inner_content = malloc(len - 1); // Espacio para el contenido sin comillas
            if (len >= 2) {
                strncpy(inner_content, nodo->valor + 1, len - 2);
            }
            inner_content[len - 2] = '\0';
            
            char* processed_string = process_string_escapes(inner_content);
            
            free(inner_content);
            free(nodo->valor);
            return nuevoValorResultado((void*) processed_string, STRING);
        }
        case CHAR: {
            int* valor_char_unicode = malloc(sizeof(int));
            if (strlen(nodo->valor) >= 3) {
                if (nodo->valor[1] == '\\' && nodo->valor[2] == 'u') {
                    char hex_code[5] = {0};
                    strncpy(hex_code, nodo->valor + 3, 4);
                    *valor_char_unicode = (int)strtol(hex_code, NULL, 16);
                } else if (nodo->valor[1] == '\\') {
                    switch(nodo->valor[2]) {
                        case 'n': *valor_char_unicode = '\n'; break;
                        case 't': *valor_char_unicode = '\t'; break;
                        case 'r': *valor_char_unicode = '\r'; break;
                        case '\'': *valor_char_unicode = '\''; break;
                        case '\"': *valor_char_unicode = '\"'; break;
                        case '\\': *valor_char_unicode = '\\'; break;
                        default: *valor_char_unicode = nodo->valor[2]; break;
                    }
                } else {
                    *valor_char_unicode = (int)nodo->valor[1];
                }
            } else {
                *valor_char_unicode = 0;
            }
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_char_unicode, nodo->tipo);
        }
        case BOOLEAN: {
            int* valor_bool = malloc(sizeof(int));
            *valor_bool = (strcmp(nodo->valor, "true") == 0);
            free(nodo->valor);
            return nuevoValorResultado((void*) valor_bool, nodo->tipo);
        }
        default:
            printf("Tipo de dato primitivo no implementado");
    }
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoPrimitivoExpresion(char* v, TipoDato tipo) {
    PrimitivoExpresion* nodo = malloc(sizeof(PrimitivoExpresion));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretPrimitivoExpresion, "Primitivo");
    nodo->valor = v;
    nodo->tipo = tipo;
    return (AbstractExpresion*) nodo;
}
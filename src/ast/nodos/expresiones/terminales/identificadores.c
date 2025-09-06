#include "identificadores.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // <<<< AÑADIDO: Soluciona el warning de 'snprintf'

// Función auxiliar para obtener el tamaño en bytes de nuestros tipos de datos
static size_t get_type_size(TipoDato tipo)
{
    switch (tipo)
    {
    case INT:
    case BOOLEAN:
    case CHAR:
        return sizeof(int);
    case FLOAT:
        return sizeof(float);
    case DOUBLE:
        return sizeof(double);
    default:
        return 0; // String se maneja por separado
    }
}

// Función de interpretación (la que ya tenías)
Result interpretIdentificadorExpresion(AbstractExpresion *self, Context *context)
{
    IdentificadorExpresion *nodo = (IdentificadorExpresion *)self;
    Symbol *simbolo = buscarTablaSimbolos(context, nodo->nombre);

    if (!simbolo)
    {
        char description[256];
        snprintf(description, sizeof(description), "La variable '%s' no ha sido declarada.", nodo->nombre);
        add_error_to_report("Semantico", nodo->nombre, description, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    // Si la variable es un arreglo, devolvemos la REFERENCIA al arreglo.
    // Importante: No copiamos estructuras de arreglo; otros nodos (como acceso [])
    // deben tratarlo como referencia sin liberarlo.
    if (simbolo->tipo == ARRAY)
    {
        return nuevoValorResultado(simbolo->info.var.valor, ARRAY);
    }

    // <<<< CORREGIDO: Accedemos al valor a través de la union >>>>
    if (simbolo->info.var.valor == NULL)
    {
        return nuevoValorResultado(NULL, simbolo->tipo);
    }

    // Devolvemos una COPIA del valor, protegiendo la tabla de símbolos.
    if (simbolo->tipo == STRING)
    {
        // <<<< CORREGIDO: Accedemos al valor a través de la union >>>>
        return nuevoValorResultado(strdup((char *)simbolo->info.var.valor), STRING);
    }
    else
    {
        size_t size = get_type_size(simbolo->tipo);
        void *valor_copiado = malloc(size);
        if (!valor_copiado)
            return nuevoValorResultadoVacio();

        // <<<< CORREGIDO: Accedemos al valor a través de la union >>>>
        memcpy(valor_copiado, simbolo->info.var.valor, size);
        return nuevoValorResultado(valor_copiado, simbolo->tipo);
    }
}

// <<<< FUNCIÓN CONSTRUCTORA AÑADIDA >>>>
AbstractExpresion *nuevoIdentificadorExpresion(char *nombre, int line, int column)
{
    IdentificadorExpresion *nodo = malloc(sizeof(IdentificadorExpresion));
    if (!nodo)
        return NULL;

    buildAbstractExpresion(&nodo->base, interpretIdentificadorExpresion, "Identificador", line, column);
    nodo->nombre = nombre;
    nodo->line = line;
    nodo->column = column;

    return (AbstractExpresion *)nodo;
}
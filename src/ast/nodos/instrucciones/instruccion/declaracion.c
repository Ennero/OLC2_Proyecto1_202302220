#include "declaracion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "context/result.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast/nodos/estructuras/arreglos/inicializador_arreglo.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Result interpretDeclaracionVariable(AbstractExpresion *nodo, Context *context)
{
    DeclaracionVariable *self = (DeclaracionVariable *)nodo;
    TipoDato tipo_declarado = (self->dimensiones > 0) ? ARRAY : self->tipo;
    // DEBUG
    // fprintf(stderr, "[DEBUG] Declaracion: nombre=%s, tipo=%d, dims=%d, hijos=%zu\n", self->nombre, self->tipo, self->dimensiones, nodo->numHijos);

    // Caso 1: Hay una expresión de asignación (ej. = new int[5] o = {1,2})
    if (nodo->numHijos > 0)
    {
    // fprintf(stderr, "[DEBUG] Declaracion: interpretando hijo 0 tipo=%s\n", nodo->hijos[0]->node_type);
    Result resultado = nodo->hijos[0]->interpret(nodo->hijos[0], context);
    // fprintf(stderr, "[DEBUG] Declaracion: hijo interpretado, res.tipo=%d, res.valor=%p\n", resultado.tipo, resultado.valor);

        // Sub-caso 1.1: Inicialización con {...}
        if (strcmp(nodo->hijos[0]->node_type, "ArrayInitializer") == 0)
        {
            if (self->dimensiones == 0)
            {
                add_error_to_report("Semantico", self->nombre, "La sintaxis de inicializador de arreglo solo es válida para declarar arreglos.", self->line, self->column, context->nombre_completo);
                return nuevoValorResultadoVacio();
            }
            AbstractExpresion *inicializador_nodo = (AbstractExpresion *)resultado.valor;
            if (!inicializador_nodo) {
                add_error_to_report("Semantico", self->nombre, "Inicializador de arreglo inválido (nulo).", self->line, self->column, context->nombre_completo);
                return nuevoValorResultadoVacio();
            }
            AbstractExpresion* lista = (inicializador_nodo->numHijos > 0) ? inicializador_nodo->hijos[0] : NULL;
            if (!lista) {
                add_error_to_report("Semantico", self->nombre, "Inicializador de arreglo vacío o mal construido.", self->line, self->column, context->nombre_completo);
                return nuevoValorResultadoVacio();
            }
            // fprintf(stderr, "[DEBUG] Declaracion: construyendo arreglo desde inicializador...\n");
            ArrayValue *arr_val = construirDesdeInicializador(self, context, lista, self->tipo, self->dimensiones);
            // fprintf(stderr, "[DEBUG] Declaracion: arreglo construido %p\n", (void*)arr_val);
            if (!arr_val) {
                return nuevoValorResultadoVacio();
            }
            agregarSymbol(context, nuevoVariable(self->nombre, arr_val, ARRAY), self->line, self->column);
        }
        // Sub-caso 1.2: Inicialización con 'new' u otra variable
        else
        {
            if (tipo_declarado != resultado.tipo)
            {
                char desc[256];
                snprintf(desc, sizeof(desc), "Error de tipos: no se puede asignar un valor de tipo '%s' a la nueva variable '%s' de tipo '%s'.",
                        labelTipoDato[resultado.tipo], self->nombre, labelTipoDato[tipo_declarado]);
                add_error_to_report("Semantico", self->nombre, desc, self->line, self->column, context->nombre_completo);
                return nuevoValorResultadoVacio();
            }
            agregarSymbol(context, nuevoVariable(self->nombre, resultado.valor, tipo_declarado), self->line, self->column);
        }
    }
    // Caso 2: Sin asignación (ej. int x; o int[] a;)
    else
    {
        void *default_value = NULL;
        if (tipo_declarado != ARRAY)
        { // Si es primitivo, crea valor por defecto
            default_value = getDefaultValueForType(self->tipo).valor;
        } // Si es arreglo, el valor por defecto es NULL
        agregarSymbol(context, nuevoVariable(self->nombre, default_value, tipo_declarado), self->line, self->column);
    }
    return nuevoValorResultadoVacio();
}

// Constructor actualizado
AbstractExpresion *nuevoDeclaracionVariable(TipoDato tipo, int dimensiones, char *nombre, AbstractExpresion *expresion, int line, int column)
{
    DeclaracionVariable *nodo = malloc(sizeof(DeclaracionVariable));
    buildAbstractExpresion(&nodo->base, interpretDeclaracionVariable, "Declaracion", line, column);
    nodo->tipo = tipo;
    nodo->dimensiones = dimensiones;
    nodo->nombre = nombre;
    nodo->line = line;
    nodo->column = column;
    if (expresion)
        agregarHijo((AbstractExpresion *)nodo, expresion);
    return (AbstractExpresion *)nodo;
}
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

    if (nodo->numHijos > 0) {
        Result resultado = nodo->hijos[0]->interpret(nodo->hijos[0], context);

        if (strcmp(nodo->hijos[0]->node_type, "ArrayInitializer") == 0) {
            if (self->dimensiones == 0) {
                add_error_to_report("Semantico", self->nombre, "La sintaxis de inicializador de arreglo solo es válida para declarar arreglos.", self->line, self->column, context->nombre_completo);
                return nuevoValorResultadoVacio();
            }
            
            AbstractExpresion *inicializador_nodo = (AbstractExpresion *)resultado.valor;
            ArrayValue *arr_val = NULL;

            // --- INICIO DE LA CORRECCIÓN ---
            if (inicializador_nodo->numHijos == 0) {
                // CASO: Inicializador vacío int[] a = {};
                // Creamos un arreglo de tamaño 0.
                arr_val = nuevoArray(self->tipo, self->dimensiones, 0);
            } else {
                // CASO: Inicializador con elementos int[] a = {1, 2};
                // Procedemos como antes.
                AbstractExpresion* lista_de_expresiones = inicializador_nodo->hijos[0];
                arr_val = construirDesdeInicializador(self, context, lista_de_expresiones, self->tipo, self->dimensiones);
            }
            // --- FIN DE LA CORRECCIÓN ---

            if (!arr_val) {
                // Si la construcción falló por un error de tipos, retornamos.
                return nuevoValorResultadoVacio();
            }
            agregarSymbol(context, nuevoVariable(self->nombre, arr_val, ARRAY), self->line, self->column);

        } else {
            if (tipo_declarado != resultado.tipo) {
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
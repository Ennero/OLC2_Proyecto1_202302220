#include "asignacion_compuesta.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// Importamos las tablas de operaciones de los otros módulos
#include "ast/nodos/expresiones/aritmeticas/aritmeticas.h"
#include "ast/nodos/expresiones/bit/bitwise.h"

// Función para obtener la tabla de operaciones correcta según el operador
static Operacion (*get_op_table(char op_type))[TIPO_COUNT][TIPO_COUNT]
{
    switch (op_type)
    {
    case '+':
        return &tablaOperacionesSuma;
    case '-':
        return &tablaOperacionesResta;
    case '*':
        return &tablaOperacionesMultiplicacion;
    case '/':
        return &tablaOperacionesDivision;
    case '%':
        return &tablaOperacionesModulo;
    // Se podrían añadir aquí los de bit a bit si se implementaran (ej. &=)
    default:
        return NULL;
    }
}

// La función que interpreta una asignación compuesta
Result interpretAsignacionCompuesta(AbstractExpresion *self, Context *context)
{
    AsignacionCompuestaExpresion *nodo = (AsignacionCompuestaExpresion *)self;

    // 1. Buscar si la variable existe
    Symbol *simbolo = buscarTablaSimbolos(context, nodo->nombre);
    if (!simbolo)
    {
        char desc[256];
        sprintf(desc, "La variable '%s' no ha sido declarada.", nodo->nombre);
        add_error_to_report("Semántico", nodo->nombre, desc, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    // 2. Crear un "ExpresionLenguaje" temporal para simular la operación
    ExpresionLenguaje expr_simulada;
    // El lado izquierdo es el valor actual de la variable
    expr_simulada.izquierda = nuevoValorResultado(simbolo->valor, simbolo->tipo);
    // El lado derecho es el resultado de la expresión
    expr_simulada.derecha = self->hijos[0]->interpret(self->hijos[0], context);

    if (has_semantic_error_been_found())
        return nuevoValorResultadoVacio();

    // 3. Buscar la operación correcta en la tabla correspondiente
    Operacion(*tabla)[TIPO_COUNT][TIPO_COUNT] = get_op_table(nodo->op_type);
    if (!tabla)
    { /* Error interno, no debería ocurrir */
        return nuevoValorResultadoVacio();
    }

    Operacion op = (*tabla)[expr_simulada.izquierda.tipo][expr_simulada.derecha.tipo];
    if (!op)
    {
        char desc[256];
        sprintf(desc, "El operador '%c=' no está definido entre los tipos '%s' y '%s'.", nodo->op_type,
                labelTipoDato[expr_simulada.izquierda.tipo], labelTipoDato[expr_simulada.derecha.tipo]);
        add_error_to_report("Semántico", nodo->nombre, desc, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    // 4. Ejecutar la operación
    Result nuevo_resultado = op(&expr_simulada);

        // 5. Validar que el tipo resultante se pueda asignar a la variable original
    bool es_compatible = (nuevo_resultado.tipo == simbolo->tipo);

    // <<<< NUEVA REGLA DE COMPATIBILIDAD >>>>
    // Se permite asignar un resultado INT a una variable CHAR,
    // simulando el casteo implícito de Java en estas operaciones.
    if (simbolo->tipo == CHAR && nuevo_resultado.tipo == INT) {
        es_compatible = true;
    }

    // La regla para la concatenación de strings sigue siendo válida
    if (simbolo->tipo == STRING && nuevo_resultado.tipo == STRING) {
        es_compatible = true;
    }

    // Si después de las excepciones no es compatible, se reporta el error
    if (!es_compatible) {
        char desc[256];
        sprintf(desc, "Error de tipos: no se puede asignar un resultado de tipo '%s' a la variable '%s' de tipo '%s'.",
            labelTipoDato[nuevo_resultado.tipo], nodo->nombre, labelTipoDato[simbolo->tipo]);
        add_error_to_report("Semántico", nodo->nombre, desc, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    // 6. Actualizar el valor en la tabla de símbolos
    if (simbolo->valor)
        free(simbolo->valor);
    simbolo->valor = nuevo_resultado.valor;

    return nuevoValorResultadoVacio();
}

// El constructor para el nodo de asignación compuesta
AbstractExpresion *nuevoAsignacionCompuestaExpresion(char *nombre, char op_type, AbstractExpresion *expresion, int line, int column)
{
    AsignacionCompuestaExpresion *nodo = malloc(sizeof(AsignacionCompuestaExpresion));
    if (!nodo)
        return NULL;

    buildAbstractExpresion(&nodo->base, interpretAsignacionCompuesta, "AsignacionCompuesta", line, column);
    nodo->nombre = nombre;
    nodo->op_type = op_type;
    nodo->line = line;
    nodo->column = column;

    agregarHijo((AbstractExpresion *)nodo, expresion);

    return (AbstractExpresion *)nodo;
}
#include "asignacion_compuesta.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// Importamos las tablas de operaciones
#include "ast/nodos/expresiones/aritmeticas/aritmeticas.h"
#include "ast/nodos/expresiones/bit/bitwise.h"
#include "parser.tab.h" // Necesario para los valores de los tokens

// Función para obtener la tabla de operaciones correcta
static Operacion (*get_op_table(int op_type))[TIPO_COUNT][TIPO_COUNT]
{
    switch (op_type)
    {
        case '+': return &tablaOperacionesSuma;
        case '-': return &tablaOperacionesResta;
        case '*': return &tablaOperacionesMultiplicacion;
        case '/': return &tablaOperacionesDivision;
        case '%': return &tablaOperacionesModulo;
        case '&': return &tablaOperacionesBitwiseAnd;
        case '|': return &tablaOperacionesBitwiseOr;
        case '^': return &tablaOperacionesBitwiseXor;
        case TOKEN_LSHIFT: return &tablaOperacionesLeftShift;
        case TOKEN_RSHIFT: return &tablaOperacionesRightShift;
        case TOKEN_URSHIFT: return &tablaOperacionesUnsignedRightShift;
        default: return NULL;
    }
}

// Función para obtener el string del operador para los mensajes de error
static const char* get_op_string(int op_type) {
    switch(op_type) {
        case '+': return "+="; case '-': return "-="; case '*': return "*=";
        case '/': return "/="; case '%': return "%="; case '&': return "&=";
        case '|': return "|="; case '^': return "^="; case TOKEN_LSHIFT: return "<<=";
        case TOKEN_RSHIFT: return ">>="; case TOKEN_URSHIFT: return ">>>=";
        default: return "?=";
    }
}


// La función que interpreta una asignación compuesta (lógica sin cambios)
Result interpretAsignacionCompuesta(AbstractExpresion *self, Context *context)
{
    AsignacionCompuestaExpresion *nodo = (AsignacionCompuestaExpresion *)self;
    Symbol *simbolo = buscarTablaSimbolos(context, nodo->nombre);
    if (!simbolo) { /* ... manejo de error de variable no declarada ... */ }

    ExpresionLenguaje expr_simulada;
    expr_simulada.izquierda = nuevoValorResultado(simbolo->valor, simbolo->tipo);
    expr_simulada.derecha = self->hijos[0]->interpret(self->hijos[0], context);

    if (has_semantic_error_been_found()) return nuevoValorResultadoVacio();

    Operacion(*tabla)[TIPO_COUNT][TIPO_COUNT] = get_op_table(nodo->op_type);
    if (!tabla) { return nuevoValorResultadoVacio(); }

    Operacion op = (*tabla)[expr_simulada.izquierda.tipo][expr_simulada.derecha.tipo];
    if (!op) {
        char desc[256];
        sprintf(desc, "El operador '%s' no está definido entre los tipos '%s' y '%s'.", get_op_string(nodo->op_type),
                labelTipoDato[expr_simulada.izquierda.tipo], labelTipoDato[expr_simulada.derecha.tipo]);
        add_error_to_report("Semántico", nodo->nombre, desc, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    Result nuevo_resultado = op(&expr_simulada);
    
    bool es_compatible = (nuevo_resultado.tipo == simbolo->tipo);
    if (simbolo->tipo == CHAR && nuevo_resultado.tipo == INT) es_compatible = true;
    if (simbolo->tipo == STRING && nuevo_resultado.tipo == STRING) es_compatible = true;

    if (!es_compatible) {
        char desc[256];
        sprintf(desc, "Error de tipos: no se puede asignar un resultado de tipo '%s' a la variable '%s' de tipo '%s'.",
            labelTipoDato[nuevo_resultado.tipo], nodo->nombre, labelTipoDato[simbolo->tipo]);
        add_error_to_report("Semántico", nodo->nombre, desc, nodo->line, nodo->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    if (simbolo->valor) free(simbolo->valor);
    simbolo->valor = nuevo_resultado.valor;

    return nuevoValorResultadoVacio();
}




// El constructor para el nodo de asignación compuesta
AbstractExpresion *nuevoAsignacionCompuestaExpresion(char *nombre, int op_type, AbstractExpresion *expresion, int line, int column)
{
    AsignacionCompuestaExpresion *nodo = malloc(sizeof(AsignacionCompuestaExpresion));
    if (!nodo) return NULL;

    buildAbstractExpresion(&nodo->base, interpretAsignacionCompuesta, "AsignacionCompuesta", line, column);
    nodo->nombre = nombre;
    nodo->op_type = op_type;
    nodo->line = line;
    nodo->column = column;

    agregarHijo((AbstractExpresion *)nodo, expresion);

    return (AbstractExpresion *)nodo;
}
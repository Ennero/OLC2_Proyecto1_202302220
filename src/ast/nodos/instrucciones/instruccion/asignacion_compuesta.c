#include "asignacion_compuesta.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Importamos las tablas de operaciones y tokens
#include "ast/nodos/expresiones/aritmeticas/aritmeticas.h"
#include "ast/nodos/expresiones/bit/bitwise.h"
#include "parser.tab.h"  Necesario para los valores de los tokens

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

static size_t get_type_size(TipoDato tipo) {
    switch(tipo) {
        case INT: case BOOLEAN: case CHAR: return sizeof(int);
        case FLOAT: return sizeof(float);
        case DOUBLE: return sizeof(double);
        default: return 0;
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


Result interpretAsignacionCompuesta(AbstractExpresion *self, Context *context)
{
    AsignacionCompuestaExpresion *nodo = (AsignacionCompuestaExpresion *)self;
    Symbol *simbolo = buscarTablaSimbolos(context, nodo->nombre);

    if (!simbolo || simbolo->clase != VARIABLE) {
        // ... (Manejo de error si no es una variable declarada)
        return nuevoValorResultadoVacio();
    }

    ExpresionLenguaje expr_simulada;
    
    // <<<< CORREGIDO: Leemos el valor a través de la union >>>>
    // Creamos una copia del valor actual para la operación, para no modificar el original prematuramente.
    void* valor_izquierdo_copiado;
    if (simbolo->tipo == STRING) {
        valor_izquierdo_copiado = strdup((char*)simbolo->info.var.valor);
    } else {
        size_t size = get_type_size(simbolo->tipo); // Necesitarás una función auxiliar get_type_size
        valor_izquierdo_copiado = malloc(size);
        memcpy(valor_izquierdo_copiado, simbolo->info.var.valor, size);
    }
    
    expr_simulada.izquierda = nuevoValorResultado(valor_izquierdo_copiado, simbolo->tipo);
    expr_simulada.derecha = self->hijos[0]->interpret(self->hijos[0], context);

    if (has_semantic_error_been_found()) { 
        free(expr_simulada.izquierda.valor); 
        free(expr_simulada.derecha.valor); 
        return nuevoValorResultadoVacio(); 
    }

    Operacion(*tabla)[TIPO_COUNT][TIPO_COUNT] = get_op_table(nodo->op_type);
    if (!tabla) { /* ... manejo de error ... */ return nuevoValorResultadoVacio(); }

    Operacion op = (*tabla)[expr_simulada.izquierda.tipo][expr_simulada.derecha.tipo];
    if (!op) {
        // ... (Tu código de error de tipos de operación está bien)
        free(expr_simulada.izquierda.valor); free(expr_simulada.derecha.valor);
        return nuevoValorResultadoVacio();
    }

    Result nuevo_resultado = op(&expr_simulada);
    
    // El intérprete de expresiones binarias ya libera los operandos, así que no necesitamos hacerlo aquí.

    // ... (Tu código de validación de compatibilidad de tipos está bien) ...

    // <<<< CORREGIDO: Actualizamos el valor a través de la union >>>>
    if (simbolo->info.var.valor) free(simbolo->info.var.valor);
    simbolo->info.var.valor = nuevo_resultado.valor;
    simbolo->tipo = nuevo_resultado.tipo; // Actualizamos el tipo por si cambió

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
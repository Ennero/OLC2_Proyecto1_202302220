// en: src/ast/nodos/embebidas/parse_int.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h> // Para strtol
#include <errno.h>  // Para verificar errores de strtol
#include <limits.h> // Para INT_MAX, INT_MIN

Result interpretParseInt(AbstractExpresion* self, Context* context) {
    // 1. Interpretar el argumento
    Result arg = self->hijos[0]->interpret(self->hijos[0], context);

    // 2. Validar que el argumento sea un String
    if (arg.tipo != STRING) {
        add_error_to_report("Semantico", "Integer.parseInt", "El argumento debe ser de tipo String.", self->line, self->column, context->nombre_completo);
        free(arg.valor);
        return nuevoValorResultadoVacio();
    }
    
    char* endptr;
    errno = 0; // Reiniciar el indicador de error
    long val = strtol((char*)arg.valor, &endptr, 10);

    // 3. Verificar errores de conversión
    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
        add_error_to_report("Semantico", "Integer.parseInt", "El número está fuera del rango de un entero.", self->line, self->column, context->nombre_completo);
        free(arg.valor);
        return nuevoValorResultadoVacio();
    }
    if (endptr == (char*)arg.valor || *endptr != '\0') {
        add_error_to_report("Semantico", "Integer.parseInt", "El string no tiene un formato de entero válido.", self->line, self->column, context->nombre_completo);
        free(arg.valor);
        return nuevoValorResultadoVacio();
    }

    // 4. Si todo está bien, crear el resultado
    free(arg.valor); // Liberamos el string original
    int* resultado = malloc(sizeof(int));
    *resultado = (int)val;
    return nuevoValorResultado(resultado, INT);
}

AbstractExpresion* nuevoParseIntExpresion(AbstractExpresion* expr, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretParseInt, "ParseInt", line, column);
    agregarHijo(nodo, expr);
    return nodo;
}
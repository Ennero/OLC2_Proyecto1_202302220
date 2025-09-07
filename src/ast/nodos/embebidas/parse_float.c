#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h> // Para strtof
#include <errno.h>  // Para verificar errores de strtof
#include <math.h>   // Para HUGE_VALF

Result interpretParseFloat(AbstractExpresion* self, Context* context) {
    // 1. Interpretar el argumento
    Result arg = self->hijos[0]->interpret(self->hijos[0], context);

    // 2. Validar que sea un String
    if (arg.tipo != STRING) {
        add_error_to_report("Semantico", "Float.parseFloat", "El argumento debe ser de tipo String.", self->line, self->column, context->nombre_completo);
        free(arg.valor);
        return nuevoValorResultadoVacio();
    }
    
    char* endptr;
    errno = 0;
    float val = strtof((char*)arg.valor, &endptr);

    // 3. Verificar errores de conversión
    if (errno == ERANGE) {
        add_error_to_report("Semantico", "Float.parseFloat", "El número está fuera del rango de un flotante.", self->line, self->column, context->nombre_completo);
        free(arg.valor);
        return nuevoValorResultadoVacio();
    }
    if (endptr == (char*)arg.valor || *endptr != '\0') {
        add_error_to_report("Semantico", "Float.parseFloat", "El string no tiene un formato de flotante válido.", self->line, self->column, context->nombre_completo);
        free(arg.valor);
        return nuevoValorResultadoVacio();
    }

    // 4. Crear el resultado
    free(arg.valor);
    float* resultado = malloc(sizeof(float));
    *resultado = val;
    return nuevoValorResultado(resultado, FLOAT);
}

AbstractExpresion* nuevoParseFloatExpresion(AbstractExpresion* expr, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretParseFloat, "ParseFloat", line, column);
    agregarHijo(nodo, expr);
    return nodo;
}
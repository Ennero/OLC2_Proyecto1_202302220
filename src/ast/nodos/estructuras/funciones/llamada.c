#include "llamada.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static size_t get_type_size(TipoDato tipo) {
    switch(tipo) {
        case INT: case BOOLEAN: case CHAR: return sizeof(int);
        case FLOAT: return sizeof(float);
        case DOUBLE: return sizeof(double);
        default: return 0; // String es un puntero, se maneja diferente
    }
}

Result interpretLlamadaFuncion(AbstractExpresion* self, Context* context) {
    LlamadaFuncionNode* nodo = (LlamadaFuncionNode*)self;
    char* nombre_func = nodo->nombre;
    AbstractExpresion* argumentos_nodo = (self->numHijos > 0) ? self->hijos[0] : NULL;

    Symbol* func_symbol = buscarTablaSimbolos(context, nombre_func);
    if (!func_symbol || func_symbol->clase != FUNCION) {
        char desc[100]; sprintf(desc, "La función '%s' no ha sido declarada.", nombre_func);
        add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }
    
    size_t num_args = argumentos_nodo ? argumentos_nodo->numHijos : 0;
    if (num_args != func_symbol->info.func.num_parametros) {
        char desc[100]; sprintf(desc, "La función '%s' esperaba %zu argumentos, pero recibió %zu.", nombre_func, func_symbol->info.func.num_parametros, num_args);
        add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    Result* argumentos_evaluados = malloc(sizeof(Result) * num_args);
    for(size_t i = 0; i < num_args; i++) {
        argumentos_evaluados[i] = argumentos_nodo->hijos[i]->interpret(argumentos_nodo->hijos[i], context);
    }

    Context* func_context = nuevoContext(context, nombre_func);
    for(size_t i = 0; i < num_args; i++) {
        Parametro param = func_symbol->info.func.parametros[i];
        if (param.tipo != argumentos_evaluados[i].tipo) {
            char desc[128];
            sprintf(desc, "Tipo de argumento %zu incompatible: se esperaba %s y se recibió %s.", i+1, labelTipoDato[param.tipo], labelTipoDato[argumentos_evaluados[i].tipo]);
            add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
        }
        
        // Creamos una copia del valor del argumento para la nueva variable
        void* valor_copiado;
        if (param.tipo == STRING) {
            valor_copiado = strdup((char*)argumentos_evaluados[i].valor);
        } else {
            size_t size = get_type_size(param.tipo);
            valor_copiado = malloc(size);
            memcpy(valor_copiado, argumentos_evaluados[i].valor, size);
        }
        // nuevoVariable duplica el nombre, no es necesario strdup aquí
    agregarSymbol(func_context, nuevoVariable(param.nombre, valor_copiado, param.tipo, 0), 0, 0);
    }
    
    for(size_t i = 0; i < num_args; i++) { free(argumentos_evaluados[i].valor); }
    free(argumentos_evaluados);

    Result res_cuerpo = func_symbol->info.func.cuerpo->interpret(func_symbol->info.func.cuerpo, func_context);

    liberarContext(func_context);

    if (res_cuerpo.tipo == RETURN_T) {
        Result* res_final_ptr = (Result*)res_cuerpo.valor;
        Result res_final = *res_final_ptr;
        free(res_final_ptr);

        if (func_symbol->info.func.tipo_retorno == NULO) {
            // Función void no debe devolver valor
            if (res_final.tipo != NULO) {
                char desc[160];
                sprintf(desc, "La función void '%s' no debe retornar un valor.", nombre_func);
                add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
                // descartamos el valor retornado para mantener consistencia
                free(res_final.valor);
                return nuevoValorResultadoVacio();
            }
            return nuevoValorResultadoVacio();
        } else {
            if (res_final.tipo != func_symbol->info.func.tipo_retorno) {
                char desc[160];
                sprintf(desc, "Tipo de retorno incompatible en '%s': se esperaba %s y se recibió %s.", nombre_func, labelTipoDato[func_symbol->info.func.tipo_retorno], labelTipoDato[res_final.tipo]);
                add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
            }
            return res_final;
        }
    }
    
    // Si la función no tuvo return:
    // - si es void (NULO), está bien y devuelve vacío;
    // - si no es void, reportamos error de falta de retorno.
    if (func_symbol->info.func.tipo_retorno == NULO) {
        return nuevoValorResultadoVacio();
    } else {
        char desc[160];
        sprintf(desc, "La función '%s' debe retornar un valor de tipo %s.", nombre_func, labelTipoDato[func_symbol->info.func.tipo_retorno]);
        add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }
}

AbstractExpresion* nuevoLlamadaFuncion(char* nombre, AbstractExpresion* args, int line, int column) {
    LlamadaFuncionNode* nodo = malloc(sizeof(LlamadaFuncionNode));
    buildAbstractExpresion(&nodo->base, interpretLlamadaFuncion, "FunctionCall", line, column);
    nodo->nombre = nombre;
    if (args) {
        agregarHijo((AbstractExpresion*)nodo, args);
    }
    return (AbstractExpresion*)nodo;
}
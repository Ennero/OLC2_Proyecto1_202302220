#include "llamada.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
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
        Result arg = argumentos_evaluados[i];

        // Validacin de tipos:
        if (param.dimensiones == 0) {
            // Parmetro escalar: tipos deben coincidir exactamente
            if (param.tipo != arg.tipo) {
                char desc[200];
                sprintf(desc, "Tipo de argumento %zu incompatible: se esperaba %s y se recibi %s.", i+1, labelTipoDato[param.tipo], labelTipoDato[arg.tipo]);
                add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
            }
        } else {
            // Parmetro arreglo: el argumento debe ser ARRAY y su base/dimensiones deben coincidir
            if (arg.tipo != ARRAY) {
                char desc[200];
                sprintf(desc, "Tipo de argumento %zu incompatible: se esperaba arreglo de %s y se recibi %s.", i+1, labelTipoDato[param.tipo], labelTipoDato[arg.tipo]);
                add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
            } else {
                ArrayValue* av = (ArrayValue*)arg.valor;
                if (av->tipo_elemento_base != param.tipo || av->dimensiones_total != param.dimensiones) {
                    char desc[240];
                    sprintf(desc, "Dimensiones/tipo base incompatibles en argumento %zu: esperado %s[%dD], recibido %s[%dD].",
                            i+1, labelTipoDato[param.tipo], param.dimensiones,
                            labelTipoDato[av->tipo_elemento_base], av->dimensiones_total);
                    add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
                }
            }
        }

        // Copia del argumento al contexto de la funcin
        void* valor_copiado = NULL;
        TipoDato tipo_variable = (param.dimensiones == 0) ? param.tipo : ARRAY;
        if (tipo_variable == STRING) {
            valor_copiado = strdup((char*)arg.valor);
        } else if (tipo_variable == ARRAY) {
            valor_copiado = copiarArray((ArrayValue*)arg.valor);
        } else {
            size_t size = get_type_size(param.tipo);
            valor_copiado = malloc(size);
            memcpy(valor_copiado, arg.valor, size);
        }
        agregarSymbol(func_context, nuevoVariable(param.nombre, valor_copiado, tipo_variable, 0), 0, 0);
    }
    
    for(size_t i = 0; i < num_args; i++) {
        if (argumentos_evaluados[i].tipo != ARRAY) {
            free(argumentos_evaluados[i].valor);
        }
    }
    free(argumentos_evaluados);

    Result res_cuerpo = func_symbol->info.func.cuerpo->interpret(func_symbol->info.func.cuerpo, func_context);

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
                liberarContext(func_context);
                return nuevoValorResultadoVacio();
            }
            liberarContext(func_context);
            return nuevoValorResultadoVacio();
        } else {
            // Verificación de tipo de retorno considerando arreglos
            int ret_dims = func_symbol->info.func.retorno_dimensiones;
            if (ret_dims == 0) {
                // Esperamos escalar del tipo exacto
                if (res_final.tipo != func_symbol->info.func.tipo_retorno) {
                    char desc[200];
                    sprintf(desc, "Tipo de retorno incompatible en '%s': se esperaba %s y se recibió %s.", nombre_func, labelTipoDato[func_symbol->info.func.tipo_retorno], labelTipoDato[res_final.tipo]);
                    add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
                }
                liberarContext(func_context);
                return res_final;
            } else {
                // Esperamos un arreglo de base 'tipo_retorno' y dimensiones 'ret_dims'
                if (res_final.tipo != ARRAY) {
                    char desc[200];
                    sprintf(desc, "Tipo de retorno incompatible en '%s': se esperaba arreglo de %s y se recibió %s.", nombre_func, labelTipoDato[func_symbol->info.func.tipo_retorno], labelTipoDato[res_final.tipo]);
                    add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
                    // liberar si corresponde
                    if (res_final.tipo != ARRAY && res_final.valor) free(res_final.valor);
                    liberarContext(func_context);
                    return nuevoValorResultadoVacio();
                }
                ArrayValue* av = (ArrayValue*)res_final.valor;
                if (!av || av->tipo_elemento_base != func_symbol->info.func.tipo_retorno || av->dimensiones_total != ret_dims) {
                    char desc[240];
                    sprintf(desc, "Tipo de retorno incompatible en '%s': esperado %s[%dD], recibido %s[%dD].",
                            nombre_func,
                            labelTipoDato[func_symbol->info.func.tipo_retorno], ret_dims,
                            av ? labelTipoDato[av->tipo_elemento_base] : "?",
                            av ? av->dimensiones_total : -1);
                    add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
                }
                // Si el arreglo retornado pertenece a una variable del contexto de la función,
                // debemos devolver una copia profunda para evitar use-after-free.
                int owned_by_context = 0;
                if (res_final.valor != NULL) {
                    for (Symbol* s = func_context->ultimoSymbol; s != NULL; s = s->anterior) {
                        if (s->clase == VARIABLE && s->tipo == ARRAY && s->info.var.valor == res_final.valor) {
                            owned_by_context = 1; break;
                        }
                    }
                }
                if (owned_by_context) {
                    ArrayValue* copia = copiarArray(av);
                    // El original será liberado al liberar el contexto de la función
                    liberarContext(func_context);
                    return nuevoValorResultado(copia, ARRAY);
                } else {
                    // Transferimos ownership del temporal al llamador
                    liberarContext(func_context);
                    return res_final;
                }
            }
        }
    }
    
    // Si la función no tuvo return:
    // - si es void (NULO), está bien y devuelve vacío;
    // - si no es void, reportamos error de falta de retorno.
    if (func_symbol->info.func.tipo_retorno == NULO) {
        liberarContext(func_context);
        return nuevoValorResultadoVacio();
    } else {
        char desc[160];
        sprintf(desc, "La función '%s' debe retornar un valor de tipo %s.", nombre_func, labelTipoDato[func_symbol->info.func.tipo_retorno]);
        add_error_to_report("Semantico", nombre_func, desc, self->line, self->column, context->nombre_completo);
        liberarContext(func_context);
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
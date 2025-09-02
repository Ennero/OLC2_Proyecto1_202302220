#include "switch.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 

// Función auxiliar para comparar dos resultados (simplificado)
int son_iguales(Result r1, Result r2) {
    if (r1.tipo != r2.tipo) return 0;

    switch(r1.tipo) {
        case INT:
        case CHAR:
        case BOOLEAN:
            return *(int*)r1.valor == *(int*)r2.valor;
        case STRING:
            // Comparamos el contenido de los strings
            return strcmp((char*)r1.valor, (char*)r2.valor) == 0;
        default:
            return 0;
    }
}


Result interpretSwitchExpresion(AbstractExpresion* self, Context* context) {
    Result switch_val = self->hijos[0]->interpret(self->hijos[0], context);

    if (has_semantic_error_been_found()) { free(switch_val.valor); return nuevoValorResultadoVacio(); }

    AbstractExpresion* case_list = self->hijos[1];
    int case_encontrado_idx = -1;
    AbstractExpresion* default_case = NULL;
    Result resultado_final = nuevoValorResultadoVacio();

    // Buscar el primer 'case' que coincida (esta parte estaba bien)
    for (size_t i = 0; i < case_list->numHijos; i++) {
        AbstractExpresion* caso = case_list->hijos[i];
        if (strcmp(caso->node_type, "DefaultCase") == 0) {
            default_case = caso;
            continue;
        }
        Result case_val = caso->hijos[0]->interpret(caso->hijos[0], context);
        if (son_iguales(switch_val, case_val)) {
            case_encontrado_idx = i;
            free(case_val.valor);
            break;
        }
        free(case_val.valor);
    }

    free(switch_val.valor);

    // Decidir desde dónde empezar a ejecutar (esta parte estaba bien)
    int inicio_ejecucion = -1;
    if (case_encontrado_idx != -1) {
        inicio_ejecucion = case_encontrado_idx;
    } else if (default_case != NULL) {
        for (size_t i = 0; i < case_list->numHijos; i++) {
            if (case_list->hijos[i] == default_case) { inicio_ejecucion = i; break; }
        }
    }
    
    // <<<< LÓGICA DE EJECUCIÓN CORREGIDA: Ahora maneja casos vacíos >>>>
    if (inicio_ejecucion != -1) {
        for (size_t i = inicio_ejecucion; i < case_list->numHijos; i++) {
            AbstractExpresion* caso_a_ejecutar = case_list->hijos[i];
            AbstractExpresion* sentencias = NULL;

            // Comprobamos si el nodo tiene sentencias antes de acceder a ellas
            if (strcmp(caso_a_ejecutar->node_type, "DefaultCase") == 0) {
                if (caso_a_ejecutar->numHijos > 0) sentencias = caso_a_ejecutar->hijos[0];
            } else { // Es un nodo "Case"
                if (caso_a_ejecutar->numHijos > 1) sentencias = caso_a_ejecutar->hijos[1];
            }
            
            // Si hay sentencias, las ejecutamos
            if (sentencias) {
                Result res_sentencia = sentencias->interpret(sentencias, context);
                if (res_sentencia.tipo == BREAK_T) {
                    break; 
                }
                if (res_sentencia.tipo == CONTINUE_T || res_sentencia.tipo == RETURN_T) {
                    resultado_final = res_sentencia;
                    break;
                }
            }
            // Si 'sentencias' es NULL (un caso vacío), simplemente continuamos al siguiente case.
        }
    }

    return resultado_final;
}



AbstractExpresion *nuevoSwitchExpresion(AbstractExpresion *expr, AbstractExpresion *case_list, int line, int column)
{
    SwitchExpresion *nodo = malloc(sizeof(SwitchExpresion));
    buildAbstractExpresion(&nodo->base, interpretSwitchExpresion, "SwitchStatement", line, column);
    agregarHijo((AbstractExpresion *)nodo, expr);
    agregarHijo((AbstractExpresion *)nodo, case_list);
    return (AbstractExpresion *)nodo;
}
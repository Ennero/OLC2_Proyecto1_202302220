// en: src/ast/nodos/embebidas/string_join.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Result interpretStringJoin(AbstractExpresion* self, Context* context) {
    Result delimitador_res = self->hijos[0]->interpret(self->hijos[0], context);
    AbstractExpresion* lista_nodo = self->hijos[1];

    if (delimitador_res.tipo != STRING) {
        add_error_to_report("Semantico", "String.join", "El primer argumento (delimitador) debe ser un String.", self->line, self->column, context->nombre_completo);
        free(delimitador_res.valor);
        return nuevoValorResultadoVacio();
    }
    char* delimitador = (char*)delimitador_res.valor;

    int num_elementos = 0;
    Result* elementos_a_unir = NULL;
    int modo_operacion = 0; // 0 = Array, 1 = Varargs
    ArrayValue* arr_container = NULL; // Puntero para liberar el contenedor del arreglo

    // Lógica de decisión
    if (lista_nodo->numHijos == 1) {
        Result res_unico_arg = lista_nodo->hijos[0]->interpret(lista_nodo->hijos[0], context);
        if (res_unico_arg.tipo == ARRAY) {
            modo_operacion = 0;
            arr_container = (ArrayValue*)res_unico_arg.valor;
            num_elementos = arr_container->tamano;
            elementos_a_unir = arr_container->valores;
        } else {
            modo_operacion = 1;
            num_elementos = 1;
            elementos_a_unir = malloc(sizeof(Result));
            elementos_a_unir[0] = res_unico_arg;
        }
    } else {
        modo_operacion = 1;
        num_elementos = lista_nodo->numHijos;
        elementos_a_unir = malloc(num_elementos * sizeof(Result));
        for (int i = 0; i < num_elementos; i++) {
            elementos_a_unir[i] = lista_nodo->hijos[i]->interpret(lista_nodo->hijos[i], context);
        }
    }

    // Lógica para construir el string (ahora completa)
    size_t total_len = 1;
    char** temp_strings = malloc(num_elementos * sizeof(char*));
    
    for (int i = 0; i < num_elementos; i++) {
        temp_strings[i] = malloc(512);
        Result elem_res = elementos_a_unir[i];
        if (elem_res.valor == NULL) snprintf(temp_strings[i], 512, "null");
        else {
             switch (elem_res.tipo) {
                case INT:     snprintf(temp_strings[i], 512, "%d", *(int*)elem_res.valor); break;
                case FLOAT:   snprintf(temp_strings[i], 512, "%f", *(float*)elem_res.valor); break;
                case DOUBLE:  snprintf(temp_strings[i], 512, "%lf", *(double*)elem_res.valor); break;
                case BOOLEAN: snprintf(temp_strings[i], 512, "%s", (*(int*)elem_res.valor) ? "true" : "false"); break;
                case CHAR:    snprintf(temp_strings[i], 512, "%c", *(int*)elem_res.valor); break;
                case STRING:  snprintf(temp_strings[i], 512, "%s", (char*)elem_res.valor); break;
                default:      snprintf(temp_strings[i], 512, ""); break;
             }
        }
        total_len += strlen(temp_strings[i]);
    }
    total_len += strlen(delimitador) * (num_elementos > 1 ? num_elementos - 1 : 0);

    char* final_string = malloc(total_len);
    final_string[0] = '\0';

    for(int i = 0; i < num_elementos; i++) {
        strcat(final_string, temp_strings[i]);
        if (i < num_elementos - 1) {
            strcat(final_string, delimitador);
        }
        free(temp_strings[i]);
    }
    free(temp_strings);
    
    // Lógica de limpieza (ahora más segura)
    free(delimitador);
    
    if (modo_operacion == 0) { // Modo Arreglo
        // No liberar el contenedor de arreglo aquí: normalmente es una referencia compartida
        // (por ejemplo, una variable), y liberar causaría double-free cuando el contexto limpie.
    } else { // Modo Varargs
        // Liberamos cada resultado que interpretamos y el arreglo temporal que los contenía.
        for (int i = 0; i < num_elementos; i++) {
            free(elementos_a_unir[i].valor);
        }
        free(elementos_a_unir);
    }

    return nuevoValorResultado(final_string, STRING);
}

// El constructor no necesita cambios
AbstractExpresion* nuevoStringJoinExpresion(AbstractExpresion* delimitador, AbstractExpresion* lista_args, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretStringJoin, "StringJoin", line, column);
    agregarHijo(nodo, delimitador);
    agregarHijo(nodo, lista_args);
    return nodo;
}
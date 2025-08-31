#include "ast_grapher.h"
#include <stdio.h>
#include <stdlib.h> // Necesario para malloc y realloc
#include <string.h>

// Headers de los nodos para acceder a sus datos
#include "ast/nodos/expresiones/terminales/primitivos.h"
#include "ast/nodos/expresiones/terminales/identificadores.h"
#include "ast/nodos/instrucciones/instruccion/declaracion.h"

// <<<< NUEVA FUNCIÓN AUXILIAR >>>>
// Escapa las comillas dobles en una cadena para que sea segura para Graphviz.
// Devuelve una nueva cadena con memoria asignada que debe ser liberada.
static char* escape_string_for_dot(const char* input) {
    if (!input) return strdup("");
    
    size_t new_len = strlen(input);
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (input[i] == '"') {
            new_len++;
        }
    }

    char* output = malloc(new_len + 1);
    if (!output) return NULL;

    size_t j = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (input[i] == '"') {
            output[j++] = '\\';
        }
        output[j++] = input[i];
    }
    output[j] = '\0';
    return output;
}


static void graph_node_declarations_recursive(AbstractExpresion* node, FILE* file) {
    if (!node) return;

    fprintf(file, "  node%p [label=\"", (void*)node);

    if (strcmp(node->node_type, "Primitivo") == 0) {
        PrimitivoExpresion* prim = (PrimitivoExpresion*)node;
        // <<<< CAMBIO CLAVE: Se escapa el valor antes de imprimirlo >>>>
        char* escaped_value = escape_string_for_dot(prim->valor);
        fprintf(file, "Primitivo\\n(%s)", escaped_value);
        free(escaped_value); // Se libera la memoria de la copia escapada
    } else if (strcmp(node->node_type, "Identificador") == 0) {
        IdentificadorExpresion* id = (IdentificadorExpresion*)node;
        fprintf(file, "ID\\n(%s)", id->nombre);
    } else if (strcmp(node->node_type, "Declaracion") == 0) {
        DeclaracionVariable* decl = (DeclaracionVariable*)node;
        fprintf(file, "Declaracion\\n(%s %s)", labelTipoDato[decl->tipo], decl->nombre);
    } else {
        fprintf(file, "%s", node->node_type);
    }

    fprintf(file, "\"];\n");

    for (size_t i = 0; i < node->numHijos; ++i) {
        graph_node_declarations_recursive(node->hijos[i], file);
    }
}

static void graph_node_connections_recursive(AbstractExpresion* node, FILE* file) {
    if (!node) return;

    for (size_t i = 0; i < node->numHijos; ++i) {
        fprintf(file, "  node%p -> node%p;\n", (void*)node, (void*)node->hijos[i]);
        graph_node_connections_recursive(node->hijos[i], file);
    }
}

void generate_ast_graph(AbstractExpresion* root, const char* output_filename) {
    if (!root) {
        printf("No se puede generar el gráfico: el AST está vacío.\n");
        return;
    }

    FILE* file = fopen(output_filename, "w");
    if (!file) {
        perror("fopen for ast graph");
        return;
    }

    fprintf(file, "digraph AST {\n");
    fprintf(file, "  graph [bgcolor=\"#333333\" fontcolor=white];\n");
    fprintf(file, "  node [shape=box, style=rounded, color=\"#5294e2\", fontcolor=white];\n");
    fprintf(file, "  edge [color=white];\n");

    graph_node_declarations_recursive(root, file);
    graph_node_connections_recursive(root, file);

    fprintf(file, "}\n");
    fclose(file);
}

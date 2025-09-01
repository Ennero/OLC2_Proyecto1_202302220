#include "ast_grapher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Headers de los nodos para acceder a sus datos
#include "ast/nodos/expresiones/terminales/primitivos.h"
#include "ast/nodos/expresiones/terminales/identificadores.h"
#include "ast/nodos/instrucciones/instruccion/declaracion.h"

// <<<< FUNCIÓN CORREGIDA: Escapa caracteres para la sintaxis de Graphviz DOT >>>>
static char* escape_string_for_dot(const char* input) {
    if (!input) return strdup("");
    
    size_t new_len = strlen(input);
    // Contar cuántos caracteres necesitan ser escapados
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (input[i] == '"' || input[i] == '\\') {
            new_len++;
        }
    }

    char* output = malloc(new_len + 1);
    if (!output) return NULL;

    size_t j = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        // Añadir una barra invertida antes de los caracteres especiales de DOT
        if (input[i] == '"' || input[i] == '\\') {
            output[j++] = '\\';
        }
        output[j++] = input[i];
    }
    output[j] = '\0';
    return output;
}

// Función recursiva para declarar los nodos
static void graph_node_declarations_recursive(AbstractExpresion* node, FILE* file) {
    if (!node) return;

    char label_buf[1024];

    if (strcmp(node->node_type, "Primitivo") == 0) {
        PrimitivoExpresion* prim = (PrimitivoExpresion*)node;
        // <<<< CAMBIO CLAVE: Se escapa el valor crudo del lexer para mostrarlo >>>>
        // Esto muestra el texto tal como fue escrito (ej. '\n', '\u0041') de forma segura.
        char* escaped_value = escape_string_for_dot(prim->valor);
        snprintf(label_buf, sizeof(label_buf), "Primitivo\\n(%s)", escaped_value);
        free(escaped_value);

        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#FFF9DB\", color=\"#B7791F\", fontcolor=\"#111111\", penwidth=1.2, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);

    } else if (strcmp(node->node_type, "Identificador") == 0) {
        IdentificadorExpresion* id = (IdentificadorExpresion*)node;
        snprintf(label_buf, sizeof(label_buf), "ID\\n(%s)", id->nombre);
        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#E8F8F0\", color=\"#2F855A\", fontcolor=\"#111111\", penwidth=1.0, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);

    } else if (strcmp(node->node_type, "Declaracion") == 0) {
        DeclaracionVariable* decl = (DeclaracionVariable*)node;
        snprintf(label_buf, sizeof(label_buf), "Declaracion\\n(%s %s)", labelTipoDato[decl->tipo], decl->nombre);
        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#EEF6FF\", color=\"#2B6CB0\", fontcolor=\"#111111\", penwidth=1.2, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);

    } else {
        snprintf(label_buf, sizeof(label_buf), "%s", node->node_type);
        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#FFFFFF\", color=\"#A0AEC0\", fontcolor=\"#111111\", penwidth=1.0, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);
    }

    // Recorrer los hijos
    for (size_t i = 0; i < node->numHijos; ++i) {
        graph_node_declarations_recursive(node->hijos[i], file);
    }
}

// Función recursiva para conectar los nodos
static void graph_node_connections_recursive(AbstractExpresion* node, FILE* file) {
    if (!node) return;
    for (size_t i = 0; i < node->numHijos; ++i) {
        fprintf(file, "  node%p -> node%p [color=\"#6B7280\", arrowhead=\"vee\", arrowsize=0.7, penwidth=1.0];\n", (void*)node, (void*)node->hijos[i]);
        graph_node_connections_recursive(node->hijos[i], file);
    }
}

// Generador público
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
    fprintf(file, "  graph [bgcolor=\"#FFFFFF\", rankdir=TB, splines=line, nodesep=0.6, ranksep=0.75, fontsize=12, fontname=\"Helvetica\"];\n");
    fprintf(file, "  node [shape=box, style=\"filled,rounded\", fillcolor=\"#FFFFFF\", color=\"#CBD5E0\", fontcolor=\"#111111\", fontname=\"Helvetica\", fontsize=11];\n");
    fprintf(file, "  edge [color=\"#6B7280\", arrowhead=\"vee\", arrowsize=0.7, penwidth=1.0];\n");

    graph_node_declarations_recursive(root, file);
    graph_node_connections_recursive(root, file);

    fprintf(file, "}\n");
    fclose(file);
}

#include "ast_grapher.h"
#include <stdio.h>
#include <stdlib.h> // Necesario para malloc y realloc
#include <string.h>

// Headers de los nodos para acceder a sus datos
#include "ast/nodos/expresiones/terminales/primitivos.h"
#include "ast/nodos/expresiones/terminales/identificadores.h"
#include "ast/nodos/instrucciones/instruccion/declaracion.h"

// --------------------------------------------------
// Pequeñas utilidades
// --------------------------------------------------
// Escapa las comillas dobles en una cadena para que sea segura para Graphviz.
// Devuelve una nueva cadena con memoria asignada que debe ser liberada.
static char* escape_string_for_dot(const char* input) {
    if (!input) return strdup("");

    size_t new_len = strlen(input);
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (input[i] == '"') {
            new_len++; // por la barra invertida
        }
    }

    char* output = malloc(new_len + 1);
    if (!output) return NULL;

    size_t j = 0;
    for (size_t i = 0; input[i] != '\0'; i++) {
        if (input[i] == '"') {
            output[j++] = '\\\\'; // produce \ en la salida C -> se interpreta como \ en dot
        }
        output[j++] = input[i];
    }
    output[j] = '\0';
    return output;
}

// --------------------------------------------------
// Generación de nodos (declaraciones) - estilo ligero y "árbol" claro
// --------------------------------------------------
static void graph_node_declarations_recursive(AbstractExpresion* node, FILE* file) {
    if (!node) return;

    // Preparar etiqueta escapada para seguridad
    char label_buf[512];

    if (strcmp(node->node_type, "Primitivo") == 0) {
        PrimitivoExpresion* prim = (PrimitivoExpresion*)node;
        char* escaped_value = escape_string_for_dot(prim->valor);
        snprintf(label_buf, sizeof(label_buf), "Primitivo\\n(%s)", escaped_value);
        free(escaped_value);

        // Nodo con color tenue (amarillo claro para primitivos)
        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#FFF9DB\", color=\"#B7791F\", fontcolor=\"#111111\", penwidth=1.2, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);

    } else if (strcmp(node->node_type, "Identificador") == 0) {
        IdentificadorExpresion* id = (IdentificadorExpresion*)node;
        char* escaped_nombre = escape_string_for_dot(id->nombre);
        snprintf(label_buf, sizeof(label_buf), "ID\\n(%s)", escaped_nombre);
        free(escaped_nombre);

        // Nodo verde claro para identificadores
        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#E8F8F0\", color=\"#2F855A\", fontcolor=\"#111111\", penwidth=1.0, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);

    } else if (strcmp(node->node_type, "Declaracion") == 0) {
        DeclaracionVariable* decl = (DeclaracionVariable*)node;
        // asegurarse de escapar nombre y tipo
        char tipo_buf[128];
        snprintf(tipo_buf, sizeof(tipo_buf), "%s", labelTipoDato[decl->tipo]);
        char* escaped_nombre = escape_string_for_dot(decl->nombre);
        char* escaped_tipo = escape_string_for_dot(tipo_buf);
        snprintf(label_buf, sizeof(label_buf), "Declaraci\u00f3n\\n(%s %s)", escaped_tipo, escaped_nombre);
        free(escaped_nombre);
        free(escaped_tipo);

        // Nodo azul claro para declaraciones
        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#EEF6FF\", color=\"#2B6CB0\", fontcolor=\"#111111\", penwidth=1.2, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);

    } else {
        // Nodo por defecto: tipo como etiqueta
        // escapamos node_type por si acaso contiene comillas
        char* escaped_type = escape_string_for_dot(node->node_type);
        snprintf(label_buf, sizeof(label_buf), "%s", escaped_type);
        free(escaped_type);

        fprintf(file,
                "  node%p [label=\"%s\", shape=box, style=\"filled,rounded\", fillcolor=\"#FFFFFF\", color=\"#A0AEC0\", fontcolor=\"#111111\", penwidth=1.0, fontsize=11, fontname=\"Helvetica\"];\n",
                (void*)node, label_buf);
    }

    // Recurse
    for (size_t i = 0; i < node->numHijos; ++i) {
        graph_node_declarations_recursive(node->hijos[i], file);
    }
}

// --------------------------------------------------
// Conexiones entre nodos - con estilo sutil para tema claro
// --------------------------------------------------
static void graph_node_connections_recursive(AbstractExpresion* node, FILE* file) {
    if (!node) return;

    for (size_t i = 0; i < node->numHijos; ++i) {
        // usamos una arista limpia, gris tenue y flecha pequeña para mantener la legibilidad
        fprintf(file, "  node%p -> node%p [color=\"#6B7280\", arrowhead=\"vee\", arrowsize=0.7, penwidth=1.0];\n", (void*)node, (void*)node->hijos[i]);
        graph_node_connections_recursive(node->hijos[i], file);
    }
}

// --------------------------------------------------
// Generador público
// --------------------------------------------------
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

    // Cabecera del archivo DOT con tema claro y ajustes para aspecto arbóreo
    fprintf(file, "digraph AST {\n");
    fprintf(file, "  // Layout y estética general (tema claro)\n");
    fprintf(file, "  graph [bgcolor=\"#FFFFFF\", rankdir=TB, splines=line, nodesep=0.6, ranksep=0.75, fontsize=12, fontname=\"Helvetica\"];\n");
    fprintf(file, "  // Valores por defecto de nodos (se pueden sobrescribir por nodo)\n");
    fprintf(file, "  node [shape=box, style=\"filled,rounded\", fillcolor=\"#FFFFFF\", color=\"#CBD5E0\", fontcolor=\"#111111\", fontname=\"Helvetica\", fontsize=11];\n");
    fprintf(file, "  // Valores por defecto de aristas\n");
    fprintf(file, "  edge [color=\"#6B7280\", arrowhead=\"vee\", arrowsize=0.7, penwidth=1.0];\n");

    graph_node_declarations_recursive(root, file);
    graph_node_connections_recursive(root, file);

    fprintf(file, "}\n");
    fclose(file);
}

#include "context.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void inicializarContexto(Context* ctx, size_t initial_capacity) {
    ctx->output_capacity = initial_capacity;
    ctx->output = (char*) malloc(ctx->output_capacity);
    if (ctx->output) {
        ctx->output[0] = '\0'; // Iniciar como un string vacío
    }
    ctx->output_size = 0;
}

void liberarContexto(Context* ctx) {
    if (ctx->output) {
        free(ctx->output);
        ctx->output = NULL;
    }
}

// Función para agregar texto al buffer de salida, reajustando el tamaño si es necesario
void agregarAlOutput(Context* ctx, const char* text) {
    size_t text_len = strlen(text);
    if (ctx->output_size + text_len + 1 > ctx->output_capacity) {
        size_t new_capacity = (ctx->output_capacity + text_len) * 2;
        char* new_output = (char*) realloc(ctx->output, new_capacity);
        if (!new_output) {
            perror("Failed to reallocate output buffer");
            return;
        }
        ctx->output = new_output;
        ctx->output_capacity = new_capacity;
    }
    strcat(ctx->output, text);
    ctx->output_size += text_len;
}
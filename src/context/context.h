#ifndef CONTEXT_H
#define CONTEXT_H

#include <stddef.h> // NUEVO: Para size_t

typedef struct {
    // NUEVO: Buffer para la salida de la función print
    char* output;
    size_t output_size;
    size_t output_capacity;
} Context;

// NUEVO: Prototipos para manejar el contexto
void inicializarContexto(Context* ctx, size_t initial_capacity);
void liberarContexto(Context* ctx);
void agregarAlOutput(Context* ctx, const char* text);


#endif
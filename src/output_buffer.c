#include "output_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Variables estáticas
static char* buffer = NULL;
static size_t capacity = 0;
static size_t length = 0;

// Inicializa el búfer
void init_output_buffer() {
    capacity = 1024; // Capacidad inicial
    buffer = malloc(capacity);
    if (buffer) {
        buffer[0] = '\0';
    }
    length = 0;
}

// Agrega texto al final del búfer
void append_to_output(const char* text) {
    if (!buffer) {
        init_output_buffer();
    }
    size_t text_len = strlen(text);
    if (length + text_len + 1 > capacity) {
        
        // Duplicar la capacidad si no hay espacio suficiente
        size_t new_capacity = capacity * 2;
        char* new_buffer = realloc(buffer, new_capacity);
        if (!new_buffer) {
            perror("realloc output buffer");
            return;
        }
        buffer = new_buffer;
        capacity = new_capacity;
    }
    // Copia el nuevo texto al final
    strcat(buffer, text);
    length += text_len;
}

const char* get_output_buffer() {
    return buffer ? buffer : "";
}

void clear_output_buffer() {
    if (buffer) {
        buffer[0] = '\0';
    }
    length = 0;
}

void free_output_buffer() {
    free(buffer);
    buffer = NULL;
    capacity = 0;
    length = 0;
}
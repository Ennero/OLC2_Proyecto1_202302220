#include "symbol_reporter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Se añaden punteros tanto a la cabeza como a la cola de la lista
static SymbolInfo* symbol_list_head = NULL;
static SymbolInfo* symbol_list_tail = NULL; // NUEVO
static int symbol_id_counter = 0;

// ... (format_value no cambia) ...

static void format_value(char* buffer, size_t size, const void* value, TipoDato type) {
    if (value == NULL) {
        snprintf(buffer, size, "null");
        return;
    }
    switch (type) {
        case INT:
            snprintf(buffer, size, "%d", *(int*)value);
            break;
        case FLOAT:
            snprintf(buffer, size, "%f", *(float*)value);
            break;
        case STRING:
            snprintf(buffer, size, "\"%s\"", (char*)value);
            break;
        case CHAR:
            snprintf(buffer, size, "'%c'", *(char*)value);
            break;
        case BOOLEAN:
            snprintf(buffer, size, "%s", *(int*)value ? "true" : "false");
            break;
        default:
            snprintf(buffer, size, "N/A");
            break;
    }
}

void init_symbol_report() {
    symbol_list_head = NULL;
    symbol_list_tail = NULL; // Reiniciar cola
    symbol_id_counter = 0;
}

void clear_symbol_report() {
    SymbolInfo* current = symbol_list_head;
    while (current != NULL) {
        SymbolInfo* next = current->next;
        free(current->name);
        free(current->type);
        free(current->value);
        free(current->scope_name);
        free(current);
        current = next;
    }
    symbol_list_head = NULL;
    symbol_list_tail = NULL; // Reiniciar cola
    symbol_id_counter = 0;
}

void free_symbol_report() {
    clear_symbol_report();
}

void add_symbol_to_report(const Symbol* symbol, int scope_level) {
    SymbolInfo* new_info = (SymbolInfo*) malloc(sizeof(SymbolInfo));
    if (!new_info) return;

    char value_buffer[256];
    char scope_buffer[64];

    format_value(value_buffer, sizeof(value_buffer), symbol->valor, symbol->tipo);
    snprintf(scope_buffer, sizeof(scope_buffer), "Contexto %s", symbol->clase == FUNCION ? symbol->nombre : "");

    new_info->id = ++symbol_id_counter;
    new_info->name = strdup(symbol->nombre);
    new_info->type = strdup(labelTipoDato[symbol->tipo]);
    new_info->value = strdup(value_buffer);
    new_info->scope_name = strdup(scope_buffer);
    new_info->scope_level = scope_level;
    new_info->line = symbol->line;
    new_info->column = symbol->column;
    new_info->next = NULL; // El nuevo nodo es siempre el último

    // --- LÓGICA DE INSERCIÓN CORREGIDA ---
    if (symbol_list_head == NULL) {
        // Si la lista está vacía, es tanto la cabeza como la cola
        symbol_list_head = new_info;
        symbol_list_tail = new_info;
    } else {
        // Si no, se añade después de la cola actual
        symbol_list_tail->next = new_info;
        symbol_list_tail = new_info; // La nueva cola es el nuevo nodo
    }
}

const SymbolInfo* get_symbol_list() {
    return symbol_list_head;
}

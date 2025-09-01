#include "symbol_reporter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static SymbolInfo* symbol_list_head = NULL;
static SymbolInfo* symbol_list_tail = NULL;
static int symbol_id_counter = 0;

// La función format_value ahora sabe cómo manejar 'double' y 'char' Unicode
static void format_value(char* buffer, size_t size, const void* value, TipoDato type) {
    if (value == NULL) {
        snprintf(buffer, size, "null");
        return;
    }
    switch (type) {
        case INT:
            snprintf(buffer, size, "%d", *(int*)value);
            break;
        case DOUBLE: // <<<< NUEVO: Manejo de double
            snprintf(buffer, size, "%g", *(double*)value);
            break;
        case STRING:
            snprintf(buffer, size, "\"%s\"", (char*)value);
            break;
        case CHAR: // <<<< CAMBIO: Manejo de char como int (Unicode)
            // Si es un caracter ASCII imprimible, se muestra como tal.
            if (*(int*)value >= 32 && *(int*)value <= 126) {
                snprintf(buffer, size, "'%c'", *(int*)value);
            } else { // Si no, se muestra como código Unicode
                snprintf(buffer, size, "'\\u%04X'", *(int*)value);
            }
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
    symbol_list_tail = NULL;
    symbol_id_counter = 0;
}

void clear_symbol_report() {
    SymbolInfo* current = symbol_list_head;
    while (current != NULL) {
        SymbolInfo* next = current->next;
        free(current->name);
        free(current->type);
        free(current->value);
        free(current->context_name);
        free(current);
        current = next;
    }
    symbol_list_head = NULL;
    symbol_list_tail = NULL;
    symbol_id_counter = 0;
}

void free_symbol_report() {
    clear_symbol_report();
}

void add_symbol_to_report(const Symbol* symbol, const char* context_name) {
    SymbolInfo* new_info = (SymbolInfo*) malloc(sizeof(SymbolInfo));
    if (!new_info) return;

    char value_buffer[256];
    format_value(value_buffer, sizeof(value_buffer), symbol->valor, symbol->tipo);
    
    new_info->id = ++symbol_id_counter;
    new_info->name = strdup(symbol->nombre);
    new_info->type = strdup(labelTipoDato[symbol->tipo]);
    new_info->value = strdup(value_buffer);
    new_info->context_name = strdup(context_name);
    new_info->line = symbol->line;
    new_info->column = symbol->column;
    new_info->next = NULL;

    if (symbol_list_head == NULL) {
        symbol_list_head = new_info;
        symbol_list_tail = new_info;
    } else {
        symbol_list_tail->next = new_info;
        symbol_list_tail = new_info;
    }
}

const SymbolInfo* get_symbol_list() {
    return symbol_list_head;
}

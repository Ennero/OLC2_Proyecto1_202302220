#include "symbol_reporter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static SymbolInfo* symbol_list_head = NULL;
static SymbolInfo* symbol_list_tail = NULL;
static int symbol_id_counter = 0;

// Formatea el valor de un símbolo para su representación en el reporte
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
        case DOUBLE:
            snprintf(buffer, size, "%.15g", *(double*)value);
            break;
        case STRING:
            snprintf(buffer, size, "\"%s\"", (char*)value);
            break;

        // Logica para los caracteres
        case CHAR: {
            int char_val = *(int*)value;
            switch(char_val) {
                case '\n': snprintf(buffer, size, "'\\n'"); break;
                case '\t': snprintf(buffer, size, "'\\t'"); break;
                case '\r': snprintf(buffer, size, "'\\r'"); break;
                case '\'': snprintf(buffer, size, "'\\''"); break;
                case '\"': snprintf(buffer, size, "'\\\"'"); break;
                case '\\': snprintf(buffer, size, "'\\\\'"); break;
                default:
                    if (char_val >= 32 && char_val <= 126) {
                        snprintf(buffer, size, "'%c'", char_val);
                    } else {
                        snprintf(buffer, size, "'\\u%04X'", char_val);
                    }
                    break;
            }
            break;
        }
        
        case BOOLEAN:
            snprintf(buffer, size, "%s", *(int*)value ? "true" : "false");
            break;
        default:
            snprintf(buffer, size, "N/A");
            break;
    }
}

// Inicializa el reporte de símbolos
void init_symbol_report() {
    symbol_list_head = NULL;
    symbol_list_tail = NULL;
    symbol_id_counter = 0;
}

// Limpia el reporte de símbolos
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

// Libera la memoria utilizada por el reporte de símbolos
void free_symbol_report() {
    clear_symbol_report();
}

// Añade un símbolo al reporte
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

// Devuelve la lista de símbolos
const SymbolInfo* get_symbol_list() {
    return symbol_list_head;
}

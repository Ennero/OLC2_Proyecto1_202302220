#include "symbol_reporter.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// La cabeza de nuestra lista enlazada de símbolos
static SymbolInfo* symbol_list_head = NULL;

void init_symbol_report() {
    symbol_list_head = NULL;
}

void clear_symbol_report() {
    SymbolInfo* current = symbol_list_head;
    SymbolInfo* next;
    while (current != NULL) {
        next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    symbol_list_head = NULL;
}

void free_symbol_report() {
    clear_symbol_report();
}

void add_symbol_to_report(const char* name, TipoDato type, int scope_level) {
    SymbolInfo* new_info = (SymbolInfo*) malloc(sizeof(SymbolInfo));
    if (!new_info) return;

    new_info->name = strdup(name); // Copiamos el nombre
    new_info->type = type;
    new_info->scope_level = scope_level;
    new_info->next = symbol_list_head; // Lo agregamos al principio de la lista
    symbol_list_head = new_info;
}

const SymbolInfo* get_symbol_list() {
    return symbol_list_head;
}
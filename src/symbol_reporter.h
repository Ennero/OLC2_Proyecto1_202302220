#ifndef SYMBOL_REPORTER_H
#define SYMBOL_REPORTER_H

#include "context/context.h"

typedef struct SymbolInfo {
    int id;
    char* name;
    char* type;
    char* value;
    char* context_name; // <<<<<<< CAMBIO: De scope_name a context_name
    int line;
    int column;
    struct SymbolInfo* next;
} SymbolInfo;

void init_symbol_report();
void clear_symbol_report();
void free_symbol_report();
// <<<<<<< CAMBIO: Ahora recibe un const char* para el nombre del contexto
void add_symbol_to_report(const Symbol* symbol, const char* context_name);
const SymbolInfo* get_symbol_list();

#endif

#ifndef SYMBOL_REPORTER_H
#define SYMBOL_REPORTER_H

#include "context/result.h" // Para TipoDato
#include "context/context.h" // Necesitamos la definición de Symbol

typedef struct SymbolInfo {
    int id;
    char* name;
    char* type;
    char* value;
    char* scope_name; // Cambiamos el nombre para más claridad
    int scope_level;
    int line;
    int column;
    struct SymbolInfo* next;
} SymbolInfo;

// Funciones para manejar el reporte
void init_symbol_report();
void clear_symbol_report();
void free_symbol_report();
void add_symbol_to_report(const Symbol* symbol, int scope_level);
const SymbolInfo* get_symbol_list();

#endif
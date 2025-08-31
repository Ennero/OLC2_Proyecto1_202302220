#ifndef SYMBOL_REPORTER_H
#define SYMBOL_REPORTER_H

#include "context/result.h" // Para TipoDato

// Estructura para almacenar una copia de la info de un símbolo
typedef struct SymbolInfo {
    char* name;
    TipoDato type;
    int scope_level; // El nivel de anidamiento del ámbito
    struct SymbolInfo* next;
} SymbolInfo;

// Funciones para manejar el reporte
void init_symbol_report();
void clear_symbol_report();
void free_symbol_report();
void add_symbol_to_report(const char* name, TipoDato type, int scope_level);
const SymbolInfo* get_symbol_list();

#endif
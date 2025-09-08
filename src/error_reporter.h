#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

// API del sistema de reportes de errores (lexicos, sintacticos y semanticOS)
#include <stdbool.h> // Para usar el tipo 'bool'

// La estructura ErrorInfo no cambia
typedef struct ErrorInfo {
    int id;
    char* type;
    char* lexeme;
    char* description;
    char* context_name;
    int line;
    int column;
    struct ErrorInfo* next;
} ErrorInfo;

// API publica
void init_error_report();
void clear_error_report();
void free_error_report();
void add_error_to_report(const char* type, const char* lexeme, const char* description, int line, int column, const char* context_name);
const ErrorInfo* get_error_list();

// Verifica si se ha encontrado al menos un error semantico
bool has_semantic_error_been_found();

#endif
#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

#include <stdbool.h> // Para poder usar el tipo 'bool'

// La estructura ErrorInfo no cambia
typedef struct ErrorInfo {
    int id;
    char* type;
    char* lexeme;
    char* description;
    int line;
    int column;
    struct ErrorInfo* next;
} ErrorInfo;

// Las funciones existentes no cambian
void init_error_report();
void clear_error_report();
void free_error_report();
void add_error_to_report(const char* type, const char* lexeme, const char* description, int line, int column);
const ErrorInfo* get_error_list();

// Funcion para verificar si se ha encontrado un error semántico
bool has_semantic_error_been_found();

#endif
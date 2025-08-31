#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

// <<<<< CAMBIO: Se añaden nuevos campos a la estructura >>>>>
typedef struct ErrorInfo {
    int id;                 // Correlativo numérico
    char* type;             // Tipo de error (Léxico, Sintáctico)
    char* lexeme;           // El texto que causó el error
    char* description;
    int line;
    int column;
    struct ErrorInfo* next;
} ErrorInfo;

// Funciones para manejar el reporte
void init_error_report();
void clear_error_report();
void free_error_report();
// <<<<< CAMBIO: La firma ahora acepta más información >>>>>
void add_error_to_report(const char* type, const char* lexeme, const char* description, int line, int column);
const ErrorInfo* get_error_list();

#endif


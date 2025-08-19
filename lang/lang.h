#ifndef LANG_H
#define LANG_H

#include <stddef.h>

/* función principal: parsea y ejecuta 'input'.
   - out: buffer donde se escribe resultado o mensaje de error
   - out_size: tamaño de 'out'
   Devuelve 0 si OK, !=0 si hubo error (léxico/sintáctico/ejecución).
*/
int jl_run(const char *input, char *out, size_t out_size);

/* usado por el parser/lexer para reportar errores */
void jl_set_error(const char *fmt, ...);

/* variables accesibles si se necesitan (opcional) */
extern double jl_result;

#endif /* LANG_H */

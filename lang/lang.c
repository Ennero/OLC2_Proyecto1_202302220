#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "lang.h"

/* decls generadas por bison/flex */
typedef void *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE);
extern int yyparse(void);

double jl_result = 0.0;
static char jl_errbuf[512] = {0};

void jl_set_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(jl_errbuf, sizeof(jl_errbuf), fmt, ap);
    va_end(ap);
}

/* limpia el buffer de error antes de cada run */
static void jl_clear_error(void) {
    jl_errbuf[0] = '\0';
}

int jl_run(const char *input, char *out, size_t out_size) {
    jl_clear_error();
    jl_result = 0.0;

    /* crear buffer de scanning y llamar al parser */
    YY_BUFFER_STATE b = yy_scan_string(input);
    int res = yyparse();
    yy_delete_buffer(b);

    if (jl_errbuf[0] != '\0') {
        /* error léxico o sintáctico (o runtime como división por cero) */
        snprintf(out, out_size, "❌ Error: %s\n", jl_errbuf);
        return 1;
    }

    /* éxito: mostrar resultado numérico con formato sencillo */
    snprintf(out, out_size, "✅ Resultado: %.10g\n", jl_result);
    return 0;
}

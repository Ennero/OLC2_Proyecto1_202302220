#include "java_num_format.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static void ensure_decimal_point(char* buf, size_t size) {
    // If there's an 'e' or 'E', it's scientific: ensure decimal before exponent
    char* e = strpbrk(buf, "eE");
    if (e) {
        // Check if decimal point exists before exponent
        int has_dot = 0;
        for (char* p = buf; p < e; ++p) if (*p == '.') { has_dot = 1; break; }
        if (!has_dot) {
            // Insert ".0" before exponent if space allows
            size_t len = strlen(buf);
            if (len + 2 < size) {
                memmove(e + 2, e, len - (e - buf) + 1);
                e[0] = '.'; e[1] = '0';
            }
        }
        return;
    }
    // Not scientific: ensure a decimal point exists
    if (!strchr(buf, '.')) {
        size_t len = strlen(buf);
        if (len + 2 < size) { buf[len] = '.'; buf[len+1] = '0'; buf[len+2] = '\0'; }
    }
}

void java_format_double(double v, char* buf, size_t size) {
    if (isnan(v)) { snprintf(buf, size, "NaN"); return; }
    if (isinf(v)) { snprintf(buf, size, (v > 0) ? "Infinity" : "-Infinity"); return; }
    // Use sufficient precision for round-trip
    snprintf(buf, size, "%.17g", v);
    ensure_decimal_point(buf, size);
}

void java_format_float(float v, char* buf, size_t size) {
    if (isnan(v)) { snprintf(buf, size, "NaN"); return; }
    if (isinf(v)) { snprintf(buf, size, (v > 0) ? "Infinity" : "-Infinity"); return; }
    snprintf(buf, size, "%.7g", v);
    ensure_decimal_point(buf, size);
}

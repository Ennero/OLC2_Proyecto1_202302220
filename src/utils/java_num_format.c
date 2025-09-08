// Formateo numerico similar a Java (Float/Double.toString),
// incluyendo punto decimal garantizado y normalizacion de exponentes.
#include "java_num_format.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Forward declarations for helpers used below
static void ensure_decimal_point(char* buf, size_t size);
static void normalize_exponent(char* buf, size_t size);

// Helper formatters declared at file scope (C doesn't allow nested functions)
static void format_candidate_double(int k, int use_exp, double val, char* out, size_t out_size) {
    if (use_exp) {
        snprintf(out, out_size, "%.*e", k, val);
    } else {
        snprintf(out, out_size, "%.*f", k, val);
    }
    ensure_decimal_point(out, out_size);
    normalize_exponent(out, out_size);
}

static int preserves_neg_zero_double(double val, const char* s) {
    if (val == 0.0 && signbit(val)) return s && s[0] == '-';
    return 1;
}

static void format_candidate_float(int k, int use_exp, float val, char* out, size_t out_size) {
    if (use_exp) {
        snprintf(out, out_size, "%.*e", k, (double)val);
    } else {
        snprintf(out, out_size, "%.*f", k, (double)val);
    }
    ensure_decimal_point(out, out_size);
    normalize_exponent(out, out_size);
}

static int preserves_neg_zero_float(float val, const char* s) {
    if (val == 0.0f && signbit(val)) return s && s[0] == '-';
    return 1;
}

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

// Normalize exponent to Java style: use 'E', remove '+', strip leading zeros.
static void normalize_exponent(char* buf, size_t size) {
    (void)size;
    char* e = strpbrk(buf, "eE");
    if (!e) return;
    *e = 'E';

    // Remove optional '+' immediately after E
    char* p = e + 1;
    if (*p == '+') {
        memmove(p, p + 1, strlen(p));
    }

    // Now p points to '-' or the first digit
    char* d = p;
    if (*d == '-') {
        d = p + 1; // keep the minus sign
    }

    // Remove leading zeros in exponent, but keep at least one digit
    while (*d == '0' && isdigit((unsigned char)d[1])) {
        memmove(d, d + 1, strlen(d));
    }
    // If exponent ended up empty after E or E-, ensure a single zero
    if (*d == '\0') {
        *d = '0';
        d[1] = '\0';
    }
}

void java_format_double(double v, char* buf, size_t size) {
    if (isnan(v)) { snprintf(buf, size, "NaN"); return; }
    if (isinf(v)) { snprintf(buf, size, (v > 0) ? "Infinity" : "-Infinity"); return; }
    // Decide style per Java's thresholds: scientific if |v| >= 1e7 or (0 < |v| < 1e-3)
    double av = fabs(v);
    int use_exp_style = (av != 0.0) && (av >= 1e7 || av < 1e-3);

    // Prepare a safe generic fallback
    char generic[256];
    snprintf(generic, sizeof(generic), "%.17g", v);
    ensure_decimal_point(generic, sizeof(generic));
    normalize_exponent(generic, sizeof(generic));

    // Search for the shortest rounded representation that round-trips and preserves -0.0 sign
    char candidate[256];
    for (int k = 1; k <= 17; ++k) {
        format_candidate_double(k, use_exp_style, v, candidate, sizeof(candidate));
        char* endptr = NULL;
        double parsed = strtod(candidate, &endptr);
        if (endptr && *endptr == '\0' && parsed == v && preserves_neg_zero_double(v, candidate)) {
            snprintf(buf, size, "%s", candidate);
            return;
        }
    }

    // Fallback to generic
    snprintf(buf, size, "%s", generic);
    // Enforce style if thresholds demand scientific but generic chose fixed
    if (use_exp_style && !strpbrk(buf, "eE")) {
        for (int k = 1; k <= 17; ++k) {
            format_candidate_double(k, 1 /*force exp*/, v, candidate, sizeof(candidate));
            char* endptr = NULL;
            double parsed = strtod(candidate, &endptr);
            if (endptr && *endptr == '\0' && parsed == v && preserves_neg_zero_double(v, candidate)) {
                snprintf(buf, size, "%s", candidate);
                return;
            }
        }
        // Last resort: use a precise exponential form
        snprintf(buf, size, "%.17e", v);
        ensure_decimal_point(buf, size);
        normalize_exponent(buf, size);
    }
}

void java_format_float(float v, char* buf, size_t size) {
    if (isnan(v)) { snprintf(buf, size, "NaN"); return; }
    if (isinf(v)) { snprintf(buf, size, (v > 0) ? "Infinity" : "-Infinity"); return; }
    // Same thresholds as Double per Java formatting rules
    double av = fabs((double)v);
    int use_exp_style = (av != 0.0) && (av >= 1e7 || av < 1e-3);

    // Prepare a safe generic fallback
    char generic[256];
    snprintf(generic, sizeof(generic), "%.7g", v);
    ensure_decimal_point(generic, sizeof(generic));
    normalize_exponent(generic, sizeof(generic));

    char candidate[256];
    for (int k = 1; k <= 7; ++k) {
        format_candidate_float(k, use_exp_style, v, candidate, sizeof(candidate));
        char* endptr = NULL;
        float parsed = strtof(candidate, &endptr);
        if (endptr && *endptr == '\0' && parsed == v && preserves_neg_zero_float(v, candidate)) {
            snprintf(buf, size, "%s", candidate);
            return;
        }
    }

    // Fallback to generic
    snprintf(buf, size, "%s", generic);
    if (use_exp_style && !strpbrk(buf, "eE")) {
        for (int k = 1; k <= 7; ++k) {
            format_candidate_float(k, 1 /*force exp*/, v, candidate, sizeof(candidate));
            char* endptr = NULL;
            float parsed = strtof(candidate, &endptr);
            if (endptr && *endptr == '\0' && parsed == v && preserves_neg_zero_float(v, candidate)) {
                snprintf(buf, size, "%s", candidate);
                return;
            }
        }
        // Last resort: use a precise exponential form
        snprintf(buf, size, "%.7e", (double)v);
        ensure_decimal_point(buf, size);
        normalize_exponent(buf, size);
    }
}

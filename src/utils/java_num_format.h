#ifndef JAVA_NUM_FORMAT_H
#define JAVA_NUM_FORMAT_H

#include <stddef.h>

// Format numbers similar to Java's Float.toString / Double.toString
void java_format_double(double v, char* buf, size_t size);
void java_format_float(float v, char* buf, size_t size);

#endif // JAVA_NUM_FORMAT_H

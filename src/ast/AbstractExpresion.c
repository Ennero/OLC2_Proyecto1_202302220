#include "AbstractExpresion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void agregarHijo(AbstractExpresion* padre, AbstractExpresion* hijo) {
    if (!padre || !hijo) return;
    AbstractExpresion** newarr = realloc(padre->hijos, sizeof(AbstractExpresion*) * (padre->numHijos + 1));
    if (!newarr) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    padre->hijos = newarr;
    padre->hijos[padre->numHijos] = hijo;
    padre->numHijos++;
}

void liberarAST(AbstractExpresion* raiz) {
    if (!raiz) return;
    for (size_t i = 0; i < raiz->numHijos; ++i) {
        liberarAST(raiz->hijos[i]);
    }
    free(raiz->hijos);
    free(raiz);
}

// Se añade el parámetro 'node_type'
void buildAbstractExpresion(AbstractExpresion* base, Interpret interpretPuntero, const char* node_type, int line, int column) {
    base->interpret = interpretPuntero;
    base->node_type = node_type; // Se asigna el nombre
    base->hijos = NULL;
    base->numHijos = 0;
    base->line = line;
    base->column = column;
}
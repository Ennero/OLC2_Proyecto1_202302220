#ifndef RELACIONALES_H
#define RELACIONALES_H

#include "ast/AbstractExpresion.h"

/*
 * Este archivo declara los constructores para todos los nodos
 * de expresiones relacionales (==, !=, <, >, <=, >=).
 */

AbstractExpresion* nuevoIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoDiferenteExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMenorQueExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMayorQueExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMenorIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMayorIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);

#endif
#ifndef FUNCION_H
#define FUNCION_H
#include "ast/AbstractExpresion.h"

// <<<< Añadimos el campo tipo_retorno >>>>
typedef struct {
    AbstractExpresion base;
    char* nombre;
    TipoDato tipo_retorno; 
} FuncionDeclarationNode;

#endif
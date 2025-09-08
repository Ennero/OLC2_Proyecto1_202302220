#ifndef FUNCION_H
#define FUNCION_H
#include "ast/AbstractExpresion.h"

// <<<< Añadimos el campo tipo_retorno >>>>
typedef struct {
    AbstractExpresion base;
    char* nombre;
    TipoDato tipo_retorno; 
    int retorno_dimensiones; // 0 si escalar, >0 si retorna arreglo
} FuncionDeclarationNode;

#endif
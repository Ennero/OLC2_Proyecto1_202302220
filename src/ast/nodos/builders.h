#ifndef BUILDERS_H
#define BUILDERS_H

#include "ast/AbstractExpresion.h"

/*
    Este archivo actúa como un catálogo de "fábricas" (builders)
    para cada tipo de nodo que puede existir en el Árbol de Sintaxis Abstracta (AST).
    El parser lo utiliza para construir el árbol durante el análisis.
*/

/* --- Constructores de Nodos de Instrucciones y Estructura --- */
AbstractExpresion* nuevoInstruccionesExpresion(void);
AbstractExpresion* nuevoPrintExpresion(AbstractExpresion* listaExpresiones, int line, int column);
AbstractExpresion* nuevoListaExpresiones(void);
AbstractExpresion* nuevoDeclaracionVariables(TipoDato tipo, char* nombre, AbstractExpresion* expresion, int line, int column);


AbstractExpresion* nuevoReasignacionExpresion(char* nombre, AbstractExpresion* expresion, int line, int column);
AbstractExpresion* nuevoAsignacionCompuestaExpresion(char* nombre, int op_type, AbstractExpresion* expresion, int line, int column);


AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* lSentencia, int line, int column);
AbstractExpresion* nuevoCasteoExpresion(TipoDato tipo_destino, AbstractExpresion* expresion, int line, int column);

/* --- Constructores de Nodos de Expresiones Terminales (Valores) --- */
AbstractExpresion* nuevoPrimitivoExpresion(char* valor, TipoDato tipo, int line, int column);
AbstractExpresion* nuevoIdentificadorExpresion(char* nombre, int line, int column);


/* --- Constructores de Nodos de Expresiones Aritméticas --- */
AbstractExpresion* nuevoSumaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoRestaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoMultiplicacionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoDivisionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoModuloExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoUnarioExpresion(AbstractExpresion* expresion, int line, int column);

/* --- Constructores de Nodos de Expresiones Relacionales --- */
AbstractExpresion* nuevoIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoDiferenteExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoMenorQueExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoMayorQueExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoMenorIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoMayorIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoEqualsExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);


/* --- Constructores de Nodos de Expresiones Lógicas --- */
AbstractExpresion* nuevoAndExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoOrExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoNotExpresion(AbstractExpresion* expresion, int line, int column);

/* --- Constructores de Nodos de Expresiones Bit a Bit --- */
AbstractExpresion* nuevoBitwiseAndExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoBitwiseOrExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoBitwiseXorExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoBitwiseNotExpresion(AbstractExpresion* expresion, int line, int column);

// <<<< AÑADIR ESTAS LÍNEAS >>>>
AbstractExpresion* nuevoLeftShiftExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoRightShiftExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);
AbstractExpresion* nuevoUnsignedRightShiftExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha, int line, int column);


#endif


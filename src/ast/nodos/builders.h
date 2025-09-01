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
AbstractExpresion* nuevoPrintExpresion(AbstractExpresion* listaExpresiones);
AbstractExpresion* nuevoListaExpresiones(void);
AbstractExpresion* nuevoDeclaracionVariables(TipoDato tipo, char* nombre, AbstractExpresion* expresion, int line, int column);
AbstractExpresion* nuevoReasignacionExpresion(char* nombre, AbstractExpresion* expresion, int line, int column);


// <<<< NUEVO: Se añade la declaración que faltaba >>>>
AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* lSentencia);

/* --- Constructores de Nodos de Expresiones Terminales (Valores) --- */


AbstractExpresion* nuevoPrimitivoExpresion(char* valor, TipoDato tipo);
AbstractExpresion* nuevoIdentificadorExpresion(char* nombre, int line, int column);


/* --- Constructores de Nodos de Expresiones Aritméticas --- */

AbstractExpresion* nuevoSumaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoRestaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMultiplicacionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoDivisionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoModuloExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoUnarioExpresion(AbstractExpresion* expresion);

/* --- Constructores de Nodos de Expresiones Relacionales --- */
AbstractExpresion* nuevoIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoDiferenteExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMenorQueExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMayorQueExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMenorIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMayorIgualExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);

/* --- Constructores de Nodos de Expresiones Lógicas --- */
AbstractExpresion* nuevoAndExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoOrExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoNotExpresion(AbstractExpresion* expresion);


#endif


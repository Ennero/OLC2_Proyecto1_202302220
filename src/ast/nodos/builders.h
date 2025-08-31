#ifndef BUILDERS_H
#define BUILDERS_H

#include "ast/AbstractExpresion.h"

AbstractExpresion* nuevoInstruccionesExpresion(void);
AbstractExpresion* nuevoPrintExpresion(AbstractExpresion* listaExpresiones);
AbstractExpresion* nuevoPrimitivoExpresion(char* valor, char tipo);
AbstractExpresion* nuevoListaExpresiones(void);
AbstractExpresion* nuevoDeclaracionVariables(TipoDato tipo, char* nombre, AbstractExpresion* expresion, int line, int column);
AbstractExpresion* nuevoIdentificadorExpresion(char* nombre);
AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* sentencias);

/*
    Builds expresiones
*/
AbstractExpresion* nuevoSumaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoRestaExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);
AbstractExpresion* nuevoMultiplicacionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha); // NUEVO
AbstractExpresion* nuevoDivisionExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);       // NUEVO
AbstractExpresion* nuevoModuloExpresion(AbstractExpresion* izquierda, AbstractExpresion* derecha);         // NUEVO
AbstractExpresion* nuevoUnarioExpresion(AbstractExpresion* expresion);


#endif

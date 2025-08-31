#include "identificadores.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "error_reporter.h" // Se incluye para poder reportar errores

#include <stdlib.h>
#include <stdio.h>

Result interpretIdentificadorExpresion(AbstractExpresion* self, Context* context) {
    IdentificadorExpresion* nodo = (IdentificadorExpresion*) self;
    
    // Buscar el símbolo en la tabla de símbolos actual y en las anteriores.
    Symbol* valorEncontrado = buscarTablaSimbolos(context, nodo->nombre);
    
    if (valorEncontrado) {
        // Si se encuentra, se devuelve su valor y tipo.
        return nuevoValorResultado(valorEncontrado->valor, valorEncontrado->tipo);
    }
    
    // <<<< LÓGICA SEMÁNTICA CLAVE >>>>
    // Si no se encuentra el símbolo, es un error semántico.
    char description[256];
    snprintf(description, sizeof(description), "La variable '%s' no ha sido declarada en este ámbito.", nodo->nombre);
    
    // Se reporta el error con su tipo, el lexema (nombre), la descripción y la ubicación.
    add_error_to_report("Semántico", nodo->nombre, description, nodo->line, nodo->column);
    
    // Se devuelve un valor nulo para evitar que el programa crashee,
    // aunque la ejecución se detendrá igualmente gracias a la bandera de error.
    return nuevoValorResultadoVacio();
}

// <<<< CAMBIO: El constructor ahora recibe la ubicación >>>>
AbstractExpresion* nuevoIdentificadorExpresion(char* nombre, int line, int column) {
    IdentificadorExpresion* nodo = malloc(sizeof(IdentificadorExpresion));
    if (!nodo) return NULL;
    
    buildAbstractExpresion(&nodo->base, interpretIdentificadorExpresion);

    nodo->nombre = nombre;
    // Se guardan la línea y la columna en la estructura del nodo.
    nodo->line = line;
    nodo->column = column;
    
    return (AbstractExpresion*) nodo;
}

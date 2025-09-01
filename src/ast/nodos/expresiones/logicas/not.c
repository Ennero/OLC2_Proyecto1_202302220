#include "logicas.h"
#include "ast/nodos/builders.h"
#include "ast/nodos/expresiones/expresiones.h"
#include "context/result.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <stdio.h>

// --- Función de Negación Lógica ---
// Recibe directamente el 'Result' del operando.
Result logicoNotBooleano(Result res) {
    int* val = malloc(sizeof(int));
    // Se aplica la negación lógica de C (!)
    *val = !(*((int*)res.valor));
    free(res.valor); // Se libera la memoria del valor original
    return nuevoValorResultado(val, BOOLEAN);
}

// --- Tabla de Operaciones para NOT (Unaria) ---
UnaryOperacion tablaOperacionesNot[TIPO_COUNT] = {
    [BOOLEAN] = logicoNotBooleano
};

// --- Interpretador para el Nodo Unario NOT ---
Result interpretNotExpresion(AbstractExpresion* self, Context* context) {
    // 1. Interpretar la expresión hija
    Result res = self->hijos[0]->interpret(self->hijos[0], context);

    // 2. Comprobar si el tipo es BOOLEAN
    if (res.tipo != BOOLEAN) {
        char desc[256];
        sprintf(desc, "El operador unario '!' no se puede aplicar a un valor de tipo '%s'.", labelTipoDato[res.tipo]);
        // Idealmente, se pasaría la línea/columna del operador '!'
        add_error_to_report("Semántico", "!", desc, 0, 0, NULL); 
        return nuevoValorResultadoVacio();
    }
    
    // 3. Llamar a la función de negación correcta
    return tablaOperacionesNot[res.tipo](res);
}

// --- Constructor del Nodo ---
AbstractExpresion* nuevoNotExpresion(AbstractExpresion* expresion) {
    AbstractExpresion* notExpresion = malloc(sizeof(AbstractExpresion));
    if(!notExpresion) return NULL;

    buildAbstractExpresion(notExpresion, interpretNotExpresion, "Not");
    agregarHijo(notExpresion, expresion);

    return notExpresion;
}
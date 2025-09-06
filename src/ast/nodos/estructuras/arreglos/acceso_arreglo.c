// en: src/ast/nodos/estructuras/arreglos/acceso_arreglo.c
#include "ast/nodos/builders.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <string.h>

Result interpretAccesoArreglo(AbstractExpresion* self, Context* context) {
    Result res_arr = self->hijos[0]->interpret(self->hijos[0], context);
    Result res_idx = self->hijos[1]->interpret(self->hijos[1], context);

    if (res_arr.tipo != ARRAY) {
        add_error_to_report("Semantico", "[]", "Se intentó indexar una variable que no es un arreglo.", self->line, self->column, context->nombre_completo);
        free(res_arr.valor); free(res_idx.valor);
        return nuevoValorResultadoVacio();
    }
    if (res_idx.tipo != INT) {
        add_error_to_report("Semantico", "[]", "El índice de un arreglo debe ser un entero.", self->line, self->column, context->nombre_completo);
        free(res_arr.valor); free(res_idx.valor);
        return nuevoValorResultadoVacio();
    }

    ArrayValue* arr = (ArrayValue*)res_arr.valor;
    int index = *(int*)res_idx.valor;
    free(res_idx.valor);

    if (index < 0 || index >= arr->tamano) {
        add_error_to_report("Semantico", "[]", "Índice fuera de los límites del arreglo.", self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }
    
    Result elemento = arr->valores[index];
    Result copia_elemento;
    copia_elemento.tipo = elemento.tipo;

    if (elemento.tipo == ARRAY) {
        // Si el elemento es un sub-arreglo, NO lo copiamos.
        // Pasamos la referencia para que pueda ser accedido o modificado.
        copia_elemento.valor = elemento.valor;
    } else {
        // Si es un valor primitivo, SIEMPRE creamos una copia.
        if (elemento.tipo == STRING) {
            copia_elemento.valor = elemento.valor ? strdup((char*)elemento.valor) : NULL;
        } else {
            size_t size = sizeof(int); // Default para INT, CHAR, BOOLEAN
            if (elemento.tipo == FLOAT) size = sizeof(float);
            if (elemento.tipo == DOUBLE) size = sizeof(double);
            copia_elemento.valor = malloc(size);
            if(elemento.valor) memcpy(copia_elemento.valor, elemento.valor, size);
        }
    }

    // No debemos liberar 'arr' aquí: es una referencia al arreglo almacenado en la tabla de símbolos
    // u obtenido por acceso a subarreglo. Solo devolvemos el elemento/sección correspondiente.
    return copia_elemento;
}


AbstractExpresion* nuevoAccesoArreglo(AbstractExpresion* base, AbstractExpresion* indice, int line, int column) {
    AbstractExpresion* nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretAccesoArreglo, "ArrayAccess", line, column);
    agregarHijo(nodo, base);
    agregarHijo(nodo, indice);
    return nodo;
}
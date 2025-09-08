#include "bloque.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "array_value.h"
#include <stdlib.h>
#include <stdio.h>

// La función que interpreta un bloque de sentencias (CORREGIDA)
Result interpretBloqueExpresion(AbstractExpresion* self, Context* context) {
    // 1. Generar el nombre para el nuevo bloque
    char nombre_buffer[32];
    sprintf(nombre_buffer, "bloque%d", context->raiz->proximo_id_bloque);
    context->raiz->proximo_id_bloque++;

    // 2. Crear el nuevo contexto para este bloque
    Context* nuevo_contexto = nuevoContext(context, nombre_buffer);
    
    // 3. (CAMBIO CLAVE) Interpretar las sentencias y CAPTURAR su resultado
    //    Validar que exista al menos un hijo antes de indexar.
    Result res_bloque = nuevoValorResultadoVacio(); // Valor por defecto si el bloque está vacío
    if (self->numHijos > 0 && self->hijos != NULL && self->hijos[0] != NULL) {
        res_bloque = self->hijos[0]->interpret(self->hijos[0], nuevo_contexto);
    }
    
    // 4. Si el bloque está propagando un return y el valor retornado es un ARRAY
    //    cuya memoria pertenece a este contexto de bloque, debemos clonar el
    //    arreglo ANTES de liberar el contexto para evitar use-after-free aguas arriba.
    if (res_bloque.tipo == RETURN_T && res_bloque.valor) {
        Result* wrapped = (Result*)res_bloque.valor;
        if (wrapped->tipo == ARRAY && wrapped->valor) {
            int owned_by_block = 0;
            for (Symbol* s = nuevo_contexto->ultimoSymbol; s != NULL; s = s->anterior) {
                if (s->clase == VARIABLE && s->tipo == ARRAY && s->info.var.valor == wrapped->valor) {
                    owned_by_block = 1; break;
                }
            }
            if (owned_by_block) {
                ArrayValue* copia = copiarArray((ArrayValue*)wrapped->valor);
                // El original será liberado al liberar el contexto del bloque
                wrapped->valor = copia;
            }
        }
    }

    // 5. Liberar el nuevo contexto
    liberarContext(nuevo_contexto);

    // 6. (CAMBIO CLAVE) Devolver el resultado capturado del bloque.
    // Si hubo un return, res_bloque contendrá la señal RETURN_T.
    return res_bloque;
}

// El constructor del nodo de bloque (sin cambios)
AbstractExpresion* nuevoBloqueExpresion(AbstractExpresion* lSentencia, int line, int column) {
    BloqueExpresion* nodo = malloc(sizeof(BloqueExpresion));
    if (!nodo) return NULL;
    buildAbstractExpresion(&nodo->base, interpretBloqueExpresion, "Bloque", line, column);
    if(lSentencia) {
        agregarHijo((AbstractExpresion*)nodo, lSentencia);
    }
    return (AbstractExpresion*)nodo;
}
#include "ast/AbstractExpresion.h"
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/result.h"
#include "listaExpresiones.h"

#include <stdlib.h>

static Result interpretListaExpresiones(AbstractExpresion* self, Context* context) {
    // Este nodo es contenedor; no produce valor directo.
    (void)self; (void)context;
    return nuevoValorResultadoVacio();
}

AbstractExpresion* nuevoListaExpresiones() {
    //reservar el espacio en memoria y obtener el puntero a este
    ListaExpresiones* nodo = malloc(sizeof(ListaExpresiones));
    if (!nodo) return NULL;
    //asignar valores
    buildAbstractExpresion(&nodo->base, interpretListaExpresiones, "ListaExpresiones", 0, 0);

    return (AbstractExpresion*) nodo;
}
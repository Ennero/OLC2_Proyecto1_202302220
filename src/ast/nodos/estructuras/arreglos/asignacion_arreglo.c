// en: src/ast/nodos/estructuras/arreglos/asignacion_arreglo.c
#include "ast/nodos/builders.h"
#include "context/context.h"
#include "context/array_value.h"
#include "error_reporter.h"
#include <stdlib.h>
#include <string.h>

// Prototipo para acceder a la estructura del nodo Identificador
#include "ast/nodos/expresiones/terminales/identificadores.h"

// --- NUEVA FUNCIÓN AUXILIAR ---
// Esta función navega recursivamente por un nodo de acceso (ej. a[b][c])
// hasta encontrar el nombre de la variable base (ej. "a").
static AbstractExpresion *findBaseIdentifierNode(AbstractExpresion *expr)
{
    // Si el nodo actual ya es el identificador, lo hemos encontrado.
    if (strcmp(expr->node_type, "Identificador") == 0)
    {
        return expr;
    }
    // Si es un acceso a un arreglo, la base está en su primer hijo.
    if (strcmp(expr->node_type, "ArrayAccess") == 0)
    {
        return findBaseIdentifierNode(expr->hijos[0]);
    }
    // Si no es ninguno de los dos, no es un L-value válido para la asignación.
    return NULL;
}

Result interpretAsignacionArreglo(AbstractExpresion *self, Context *context)
{
    AbstractExpresion *acceso_nodo = self->hijos[0]; // El lado izquierdo, ej. numeros[i]
    AbstractExpresion *valor_nodo = self->hijos[1];  // El lado derecho

    // --- LÓGICA CORREGIDA ---

    // 1. Encontrar el nombre de la variable base del lado izquierdo.
    AbstractExpresion *id_base_nodo = findBaseIdentifierNode(acceso_nodo);
    if (!id_base_nodo)
    {
        add_error_to_report("Semantico", "=", "El lado izquierdo de la asignación no es una variable válida.", self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    char *nombre_base = ((IdentificadorExpresion *)id_base_nodo)->nombre;
    Symbol *simbolo = buscarTablaSimbolos(context, nombre_base);

    if (!simbolo || simbolo->tipo != ARRAY)
    {
        add_error_to_report("Semantico", nombre_base, "La variable no es un arreglo o no ha sido declarada.", self->line, self->column, context->nombre_completo);
        return nuevoValorResultadoVacio();
    }

    // Nota: aunque la variable arreglo sea final, en Java se permite modificar sus elementos.
    // Por ello, NO bloqueamos aquí la asignación a elementos.

    // 2. Navegar por los índices para llegar al sub-arreglo correcto.
    ArrayValue *current_array = (ArrayValue *)simbolo->info.var.valor; // ¡Puntero al original!
    AbstractExpresion *current_acceso = acceso_nodo;

    // Este bucle maneja casos como a[b][c] = valor. Se detiene en el último acceso (a[b]).
    while (strcmp(current_acceso->hijos[0]->node_type, "ArrayAccess") == 0)
    {
        Result res_idx = current_acceso->hijos[1]->interpret(current_acceso->hijos[1], context);
        int index = *(int *)res_idx.valor;
        free(res_idx.valor);
        // ... (Aquí iría la validación de índice si fuera necesario) ...
        current_array = (ArrayValue *)current_array->valores[index].valor;
        current_acceso = current_acceso->hijos[0];
    }

    // 3. Evaluar el índice final y el nuevo valor.
    Result res_idx_final = current_acceso->hijos[1]->interpret(current_acceso->hijos[1], context);
    int index_final = *(int *)res_idx_final.valor;
    free(res_idx_final.valor);

    Result nuevo_valor = valor_nodo->interpret(valor_nodo, context);

    // 4. Validar y realizar la asignación en el arreglo original.
    if (index_final < 0 || index_final >= current_array->tamano)
    {
        add_error_to_report("Semantico", "[]", "Índice fuera de los límites del arreglo en una asignación.", self->line, self->column, context->nombre_completo);
        free(nuevo_valor.valor);
        return nuevoValorResultadoVacio();
    }
    if (current_array->tipo_elemento_base != nuevo_valor.tipo)
    {
        add_error_to_report("Semantico", "=", "Tipo incompatible en la asignación al elemento del arreglo.", self->line, self->column, context->nombre_completo);
        free(nuevo_valor.valor);
        return nuevoValorResultadoVacio();
    }

    free(current_array->valores[index_final].valor);
    current_array->valores[index_final] = nuevo_valor;

    return nuevoValorResultadoVacio();
}

// El constructor no necesita cambios
AbstractExpresion *nuevoAsignacionArreglo(AbstractExpresion *base, AbstractExpresion *indice, AbstractExpresion *expr, int line, int column)
{
    AbstractExpresion *nodo = malloc(sizeof(AbstractExpresion));
    buildAbstractExpresion(nodo, interpretAsignacionArreglo, "ArrayAssignment", line, column);

    // Creamos el nodo de acceso que representa el lado izquierdo
    AbstractExpresion *acceso = nuevoAccesoArreglo(base, indice, line, column);

    // El nodo de asignación tiene 2 hijos: el acceso y la expresión a asignar
    agregarHijo(nodo, acceso);
    agregarHijo(nodo, expr);

    return nodo;
}
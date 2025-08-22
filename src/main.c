#include "ast/AbstractExpresion.h"
#include "ast/nodos/instrucciones/instrucciones.h"
#include "context/context.h"
#include <stdio.h>
#include <stdlib.h>

int yyparse(void);
extern FILE* yyin;
AbstractExpresion* ast_root = NULL;

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("fopen");
            return 1;
        }
    }

    if (yyparse() == 0) {
        if (ast_root) {
            InstruccionesExpresion* bloque = (InstruccionesExpresion*) ast_root;
            
            Context contextPadre; // MODIFICADO
            inicializarContexto(&contextPadre, 1024); // Inicializa el buffer con 1KB

            bloque->base.interpret(bloque, &contextPadre);
            
            // La salida para la GUI está en contextPadre.output
            printf("--- Salida del programa ---\n");
            printf("%s", contextPadre.output);
            printf("--- Fin de la salida ---\n");

            liberarContexto(&contextPadre); // Liberar memoria del buffer
            liberarAST(ast_root); // Es buena práctica liberar el AST
            ast_root = NULL;
            
            printf("Análisis completado.\n");
        } else {
            printf("No se pudo analizar la entrada.\n");
        }
    } else {
        fprintf(stderr, "Falló el análisis.\n");
    }

    if (yyin && yyin != stdin) {
        fclose(yyin);
    }
    return 0;
}
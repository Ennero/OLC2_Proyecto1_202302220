#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "ast/nodos/instrucciones/instrucciones.h"

/* Declaraciones generadas por Bison/Flex */
int yyparse(void);
extern FILE* yyin;
extern int yylineno;
extern char* yytext;

/* Funciones para reiniciar el estado del lexer/parser */
void yyrestart(FILE *input_file);
int yylex_destroy(void);

AbstractExpresion* ast_root = NULL;

/* Variables globales para la interfaz GTK */
GtkWidget *window;
GtkWidget *text_view_input;
GtkWidget *text_view_output;
GtkTextBuffer *buffer_input;
GtkTextBuffer *buffer_output;

/* Función para ejecutar el código */
void execute_code(GtkWidget *widget, gpointer data) {
    GtkTextIter start, end;
    char *code_text;
    
    /* Obtener el texto del área de entrada */
    gtk_text_buffer_get_bounds(buffer_input, &start, &end);
    code_text = gtk_text_buffer_get_text(buffer_input, &start, &end, FALSE);
    
    /* Limpiar el área de salida */
    gtk_text_buffer_set_text(buffer_output, "Ejecutando...\n", -1);
    
    /* Actualizar la interfaz */
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
    
    if (strlen(code_text) == 0) {
        gtk_text_buffer_set_text(buffer_output, "Error: No se proporcionó código para compilar.\n", -1);
        g_free(code_text);
        return;
    }
    
    /* CRÍTICO: Limpiar estado anterior del parser */
    ast_root = NULL;
    yylineno = 1;
    
    /* Crear archivo temporal con el código */
    FILE *temp_file = fopen("temp_input.txt", "w");
    if (!temp_file) {
        gtk_text_buffer_set_text(buffer_output, "Error: No se pudo crear archivo temporal.\n", -1);
        g_free(code_text);
        return;
    }
    
    fprintf(temp_file, "%s", code_text);
    fclose(temp_file);
    
    /* Configurar entrada para el parser */
    FILE *new_input = fopen("temp_input.txt", "r");
    if (!new_input) {
        gtk_text_buffer_set_text(buffer_output, "Error: No se pudo abrir archivo temporal.\n", -1);
        g_free(code_text);
        return;
    }
    
    /* CRÍTICO: Reiniciar completamente el lexer */
    if (yyin) {
        fclose(yyin);
    }
    yyin = new_input;
    yyrestart(yyin);
    
    /* Redirigir stdout y stderr a archivos temporales */
    FILE *output_file = fopen("temp_output.txt", "w");
    FILE *error_file = fopen("temp_error.txt", "w");
    FILE *old_stdout = stdout;
    FILE *old_stderr = stderr;
    
    if (output_file) stdout = output_file;
    if (error_file) stderr = error_file;
    
    /* Ejecutar el análisis sintáctico y la interpretación */
    char result_text[4096] = "";
    int parse_result = yyparse();
    
    /* Restaurar stdout y stderr */
    if (output_file) {
        fclose(output_file);
        stdout = old_stdout;
    }
    if (error_file) {
        fclose(error_file);
        stderr = old_stderr;
    }
    
    /* Cerrar archivo de entrada */
    fclose(yyin);
    yyin = NULL;
    
    /* Leer los resultados */
    strcpy(result_text, "=== Resultado de la ejecución ===\n\n");
    
    if (parse_result == 0) {
        strcat(result_text, "✅ Análisis sintáctico exitoso\n\n");
        
        /* Leer salida del programa */
        FILE *out_read = fopen("temp_output.txt", "r");
        if (out_read) {
            char line[256];
            strcat(result_text, "--- Salida del programa ---\n");
            while (fgets(line, sizeof(line), out_read)) {
                strcat(result_text, line);
            }
            fclose(out_read);
        }
    } else {
        strcat(result_text, "❌ Error en el análisis sintáctico\n\n");
    }
    
    /* Leer errores si los hay */
    FILE *err_read = fopen("temp_error.txt", "r");
    if (err_read) {
        char line[256];
        int has_errors = 0;
        
        while (fgets(line, sizeof(line), err_read)) {
            if (!has_errors) {
                strcat(result_text, "\n--- Errores ---\n");
                has_errors = 1;
            }
            strcat(result_text, line);
        }
        fclose(err_read);
    }
    
    /* Mostrar resultado en el área de salida */
    gtk_text_buffer_set_text(buffer_output, result_text, -1);
    
    /* Limpiar archivos temporales */
    remove("temp_input.txt");
    remove("temp_output.txt");
    remove("temp_error.txt");
    
    /* CRÍTICO: Limpiar estado del lexer para la próxima ejecución */
    yylex_destroy();
    
    g_free(code_text);
}

/* Función para limpiar el código */
void clear_code(GtkWidget *widget, gpointer data) {
    gtk_text_buffer_set_text(buffer_input, "", -1);
    gtk_text_buffer_set_text(buffer_output, "Área de salida limpiada.\n", -1);
    gtk_widget_grab_focus(text_view_input);
}

/* Función para cargar archivo */
void load_file(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Abrir archivo",
                                       GTK_WINDOW(window),
                                       action,
                                       "_Cancelar",
                                       GTK_RESPONSE_CANCEL,
                                       "_Abrir",
                                       GTK_RESPONSE_ACCEPT,
                                       NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        
        /* Leer el archivo */
        FILE *file = fopen(filename, "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            long length = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            char *content = malloc(length + 1);
            if (content) {
                fread(content, 1, length, file);
                content[length] = '\0';
                gtk_text_buffer_set_text(buffer_input, content, -1);
                free(content);
                
                char msg[512];
                snprintf(msg, sizeof(msg), "Archivo cargado: %s\n", filename);
                gtk_text_buffer_set_text(buffer_output, msg, -1);
            }
            fclose(file);
        } else {
            gtk_text_buffer_set_text(buffer_output, "Error: No se pudo abrir el archivo.\n", -1);
        }
        
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/* Función para guardar archivo */
void save_file(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Guardar archivo",
                                       GTK_WINDOW(window),
                                       action,
                                       "_Cancelar",
                                       GTK_RESPONSE_CANCEL,
                                       "_Guardar",
                                       GTK_RESPONSE_ACCEPT,
                                       NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        
        /* Obtener el texto del área de entrada */
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer_input, &start, &end);
        char *code_text = gtk_text_buffer_get_text(buffer_input, &start, &end, FALSE);
        
        /* Guardar el archivo */
        FILE *file = fopen(filename, "w");
        if (file) {
            fprintf(file, "%s", code_text);
            fclose(file);
            
            char msg[512];
            snprintf(msg, sizeof(msg), "Archivo guardado: %s\n", filename);
            gtk_text_buffer_set_text(buffer_output, msg, -1);
        } else {
            gtk_text_buffer_set_text(buffer_output, "Error: No se pudo guardar el archivo.\n", -1);
        }
        
        g_free(code_text);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/* Función para crear la interfaz */
void create_interface() {
    GtkWidget *vbox, *hbox, *hbox_buttons;
    GtkWidget *scrolled_input, *scrolled_output;
    GtkWidget *execute_button, *clear_button, *load_button, *save_button;
    GtkWidget *label_input, *label_output;
    
    /* Crear ventana principal */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Compilador - Interfaz Gráfica");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    
    /* Conectar señal de cierre */
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    /* Crear contenedor principal vertical */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    /* Área de entrada de código */
    label_input = gtk_label_new("Código fuente:");
    gtk_widget_set_halign(label_input, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), label_input, FALSE, FALSE, 0);
    
    scrolled_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_input),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_input, -1, 250);
    
    text_view_input = gtk_text_view_new();
    buffer_input = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_input));
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view_input), TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_input), text_view_input);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_input, TRUE, TRUE, 0);
    
    /* Botones */
    hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 5);
    
    execute_button = gtk_button_new_with_label("▶️ Ejecutar");
    gtk_widget_set_size_request(execute_button, 100, 35);
    g_signal_connect(execute_button, "clicked", G_CALLBACK(execute_code), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), execute_button, FALSE, FALSE, 0);
    
    clear_button = gtk_button_new_with_label("🗑️ Limpiar");
    gtk_widget_set_size_request(clear_button, 100, 35);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(clear_code), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), clear_button, FALSE, FALSE, 0);
    
    load_button = gtk_button_new_with_label("📁 Cargar");
    gtk_widget_set_size_request(load_button, 100, 35);
    g_signal_connect(load_button, "clicked", G_CALLBACK(load_file), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), load_button, FALSE, FALSE, 0);
    
    save_button = gtk_button_new_with_label("💾 Guardar");
    gtk_widget_set_size_request(save_button, 100, 35);
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_file), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), save_button, FALSE, FALSE, 0);
    
    /* Área de salida */
    label_output = gtk_label_new("Salida del programa:");
    gtk_widget_set_halign(label_output, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), label_output, FALSE, FALSE, 5);
    
    scrolled_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_output),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_output, -1, 200);
    
    text_view_output = gtk_text_view_new();
    buffer_output = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_output));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(text_view_output), TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_output), text_view_output);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_output, TRUE, TRUE, 0);
    
    /* Establecer texto inicial */
    gtk_text_buffer_set_text(buffer_input, 
        "// Escribe tu código aquí\n"
        "// Ejemplo:\n"
        "// int main() {\n"
        "//     printf(\"Hola mundo\");\n"
        "//     return 0;\n"
        "// }\n", -1);
    
    gtk_text_buffer_set_text(buffer_output, "Listo para ejecutar código...\n", -1);
    
    /* Mostrar todos los widgets */
    gtk_widget_show_all(window);
}

int main(int argc, char** argv) {
    
    /* Verificar si se ejecuta en modo consola (con argumentos) */
    if (argc > 1) {
        /* Modo consola original */
        yyin = fopen(argv[1], "r");
        if (!yyin) { 
            perror("fopen"); 
            return 1; 
        }

        if (yyparse() == 0) {
            if (ast_root) {
                InstruccionesExpresion* bloque = (InstruccionesExpresion*) ast_root;
                printf("Validado %ld", bloque->base.numHijos);
                Context contextPadre = {};
                ast_root->interpret(ast_root, &contextPadre);
                printf("Validado.\n");
                printf("Validado.\n");
            } else {
                printf("No input parsed.\n");
            }
        } else {
            fprintf(stderr, "Parsing failed.\n");
        }

        if (yyin && yyin != stdin) fclose(yyin);
        return 0;
    }
    
    /* Modo interfaz gráfica */
    gtk_init(&argc, &argv);
    
    create_interface();
    
    gtk_main();
    
    return 0;
}
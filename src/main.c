#include "ast/AbstractExpresion.h"
#include "ast/nodos/instrucciones/instrucciones.h"
#include "context/context.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

/* Declaraciones generadas por Bison/Flex */
int yyparse(void);
extern FILE *yyin;
void yyrestart(FILE *input_file); // Para reiniciar el lexer

AbstractExpresion *ast_root = NULL;

// Búfer para capturar la salida estándar
static GtkTextBuffer *output_buffer;

// Función para ejecutar el compilador con una cadena de entrada
void run_compiler(const char *input_code)
{
    // Usar un búfer en memoria para la entrada
    yyin = fmemopen((void *)input_code, strlen(input_code), "r");
    if (!yyin)
    {
        perror("fmemopen");
        return;
    }

    // Reiniciar el lexer y el parser
    yyrestart(yyin);
    if (ast_root)
    {
        liberarAST(ast_root);
        ast_root = NULL;
    }

    // Redirigir stdout para capturar la salida
    gchar *output_str = NULL;
    GIOChannel *stdout_channel = g_io_channel_unix_new(fileno(stdout));
    gsize bytes_read;

    // Crear un pipe para capturar la salida
    int pipefd[2];
    pipe(pipefd);
    int old_stdout = dup(fileno(stdout));
    dup2(pipefd[1], fileno(stdout));
    close(pipefd[1]);

    if (yyparse() == 0)
    {
        if (ast_root)
        {
            Context contextPadre = {};
            g_print("Inicio, cantidad de instrucciones: %ld \n", ast_root->numHijos);
            ast_root->interpret(ast_root, &contextPadre);
            g_print("Fin, archivo validado.\n");
        }
        else
        {
            g_print("No se pudo parsear la entrada.\n");
        }
    }
    else
    {
        g_printerr("Fallo el parseo.\n");
    }

    // Restaurar stdout
    fflush(stdout);
    dup2(old_stdout, fileno(stdout));
    close(old_stdout);

    // Leer la salida del pipe
    GIOChannel *pipe_channel = g_io_channel_unix_new(pipefd[0]);
    g_io_channel_read_to_end(pipe_channel, &output_str, &bytes_read, NULL);
    g_io_channel_shutdown(pipe_channel, TRUE, NULL);
    g_io_channel_unref(pipe_channel);

    // Mostrar la salida en el text view de salida
    gtk_text_buffer_set_text(output_buffer, output_str, -1);
    g_free(output_str);

    fclose(yyin);
}

// Callback para el botón "Ejecutar"
static void execute_code(GtkWidget *widget, gpointer data)
{
    GtkTextView *text_view = GTK_TEXT_VIEW(data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gchar *code = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    run_compiler(code);
    g_free(code);
}

// Callbacks para los botones de archivo
static void open_file(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Abrir Archivo",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Cancelar",
                                         GTK_RESPONSE_CANCEL,
                                         "_Abrir",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // Cargar contenido del archivo en el text view de entrada
        GtkTextView *text_view = g_object_get_data(G_OBJECT(widget), "input_view");
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
        gchar *contents;
        gsize length;
        if (g_file_get_contents(filename, &contents, &length, NULL))
        {
            gtk_text_buffer_set_text(buffer, contents, length);
            g_free(contents);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void save_file(GtkWidget *widget, gpointer data)
{
    // Implementación similar a "Guardar Como" por ahora
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Guardar Archivo",
                                         GTK_WINDOW(data),
                                         action,
                                         "_Cancelar",
                                         GTK_RESPONSE_CANCEL,
                                         "_Guardar",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_current_name(chooser, "sin_titulo.usl");

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        filename = gtk_file_chooser_get_filename(chooser);

        // Guardar contenido del text view de entrada en el archivo
        GtkTextView *text_view = g_object_get_data(G_OBJECT(widget), "input_view");
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(buffer, &start);
        gtk_text_buffer_get_end_iter(buffer, &end);
        gchar *contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        g_file_set_contents(filename, contents, -1, NULL);
        g_free(contents);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *scrolled_window_input, *scrolled_window_output;
    GtkWidget *text_view_input;
    GtkWidget *button_box, *execute_button, *open_button, *save_button, *save_as_button;
    GtkWidget *ast_button, *symbols_button, *errors_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Compilador");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Text view para entrada de código
    scrolled_window_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_hexpand(scrolled_window_input, TRUE);
    gtk_widget_set_vexpand(scrolled_window_input, TRUE);
    text_view_input = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window_input), text_view_input);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window_input, 0, 1, 1, 1);

    // Text view para salida
    scrolled_window_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_hexpand(scrolled_window_output, TRUE);
    gtk_widget_set_vexpand(scrolled_window_output, TRUE);
    GtkWidget *text_view_output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_output), FALSE);
    output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_output));
    gtk_container_add(GTK_CONTAINER(scrolled_window_output), text_view_output);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window_output, 1, 1, 1, 1);

    // Botones
    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(grid), button_box, 0, 0, 2, 1);

    open_button = gtk_button_new_with_label("Abrir");
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file), window);
    g_object_set_data(G_OBJECT(open_button), "input_view", text_view_input);
    gtk_container_add(GTK_CONTAINER(button_box), open_button);

    save_button = gtk_button_new_with_label("Guardar");
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_file), window);
    g_object_set_data(G_OBJECT(save_button), "input_view", text_view_input);
    gtk_container_add(GTK_CONTAINER(button_box), save_button);

    save_as_button = gtk_button_new_with_label("Guardar Como");
    g_signal_connect(save_as_button, "clicked", G_CALLBACK(save_file), window); // Reusa la misma función
    g_object_set_data(G_OBJECT(save_as_button), "input_view", text_view_input);
    gtk_container_add(GTK_CONTAINER(button_box), save_as_button);

    execute_button = gtk_button_new_with_label("Ejecutar");
    g_signal_connect(execute_button, "clicked", G_CALLBACK(execute_code), text_view_input);
    gtk_container_add(GTK_CONTAINER(button_box), execute_button);

    ast_button = gtk_button_new_with_label("Árbol AST");
    // g_signal_connect (ast_button, "clicked", G_CALLBACK (show_ast), NULL); // Implementar después
    gtk_container_add(GTK_CONTAINER(button_box), ast_button);

    symbols_button = gtk_button_new_with_label("Tabla de Símbolos");
    // g_signal_connect (symbols_button, "clicked", G_CALLBACK (show_symbols), NULL); // Implementar después
    gtk_container_add(GTK_CONTAINER(button_box), symbols_button);

    errors_button = gtk_button_new_with_label("Errores");
    // g_signal_connect (errors_button, "clicked", G_CALLBACK (show_errors), NULL); // Implementar después
    gtk_container_add(GTK_CONTAINER(button_box), errors_button);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.compiler", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
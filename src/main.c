#include <gtk/gtk.h>
#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "output_buffer.h"

// --- Declaraciones de Flex/Bison ---
typedef struct yy_buffer_state* YY_BUFFER_STATE; // Tipo para el estado del buffer
extern int yyparse(void); // Función para iniciar el análisis
extern YY_BUFFER_STATE yy_scan_string(const char * str); // Función para escanear una cadena
extern void yy_delete_buffer(YY_BUFFER_STATE b); // Función para eliminar un buffer
extern void yylex_destroy(); // Para liberar memoria del lexer

AbstractExpresion* ast_root; // Raíz del árbol de sintaxis abstracta

// Estructura para pasar los widgets a la función callback
typedef struct {
    GtkTextBuffer *input_buffer; // Buffer de entrada
    GtkTextBuffer *output_buffer; // Buffer de salida
} AppWidgets;

// Función que se ejecuta al presionar el botón "Ejecutar"
static void on_execute_clicked(GtkButton* button, gpointer user_data) {
    AppWidgets* widgets = (AppWidgets*) user_data;

    // Obtener el texto del campo de entrada
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(widgets->input_buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(widgets->input_buffer, &start, &end, FALSE);

    // Preparar y ejecutar el intérprete
    clear_output_buffer(); // Limpiamos la salida anterior
    ast_root = NULL; // Reseteamos el AST

    // Si hay texto de entrada
    if (input_text && input_text[0] != '\0') {
        YY_BUFFER_STATE buffer_state = yy_scan_string(input_text); // Escanear la cadena de entrada

        // Si el análisis es exitoso
        if (yyparse() == 0 && ast_root) {
            Context* contextPadre = nuevoContext(NULL); // Crear contexto padre
            ast_root->interpret(ast_root, contextPadre); // Interpretar el AST
        } else {
            // Si no hay AST, mostrar error
            append_to_output("Error de sintaxis o análisis.\n");
        }

        // Limpieza de Flex/Bison
        yy_delete_buffer(buffer_state);
        // liberarAST(ast_root); // IMPORTANTE: Deberías tener una función para liberar la memoria del AST aquí
    } else {
        append_to_output("No hay código para ejecutar.\n");
    }

    // Mostrar el resultado en el campo de salida
    const char* output_text = get_output_buffer();
    gtk_text_buffer_set_text(widgets->output_buffer, output_text, -1);

    // Liberar memoria
    g_free(input_text);
}


// Función que construye la interfaz gráfica
static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window, *vbox, *paned, *scrolled_input, *scrolled_output, *input_view, *output_view, *button;
    AppWidgets* widgets = (AppWidgets*) user_data;

    // Crear la ventana principal
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Mi Intérprete");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Caja vertical para organizar los widgets
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Panel divisor para redimensionar entrada/salida
    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    // Lado Izquierdo: Entrada de Texto
    scrolled_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add1(GTK_PANED(paned), scrolled_input);
    gtk_widget_set_size_request(scrolled_input, 390, -1);

    // Vista de entrada
    input_view = gtk_text_view_new();
    widgets->input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input_view));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(input_view), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scrolled_input), input_view);

    // Lado Derecho: Salida de Texto
    scrolled_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add2(GTK_PANED(paned), scrolled_output);

    // Vista de salida
    output_view = gtk_text_view_new();
    widgets->output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE); // La salida no es editable
    gtk_container_add(GTK_CONTAINER(scrolled_output), output_view);

    // Botón de Ejecutar
    button = gtk_button_new_with_label("Ejecutar");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_execute_clicked), widgets);

    gtk_widget_show_all(window);
}


// Función principal del programa ----------------------------------------------------------------------------------------------------
int main(int argc, char** argv) {
    // Inicializa buffer
    init_output_buffer();

    // Inicializa GTK
    GtkApplication *app;
    int status;

    // Reservamos memoria para los widgets que pasaremos
    AppWidgets* widgets = g_malloc(sizeof(AppWidgets));

    // Conectar la señal de activación
    app = gtk_application_new("com.tuinterprete.gtk", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), widgets);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // Liberamos memoria al cerrar
    g_free(widgets);
    free_output_buffer();
    yylex_destroy();

    // Liberar recursos de GTK
    return status;
}

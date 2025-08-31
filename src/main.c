#include <gtk/gtk.h>
#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "output_buffer.h"
#include "symbol_reporter.h" // Se incluye el nuevo reportero de símbolos

// --- Declaraciones de Flex/Bison ---
typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern int yyparse(void);
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE b);
extern void yylex_destroy();

// DEFINICIÓN de la variable global. Se quita 'extern' y se inicializa.
AbstractExpresion* ast_root = NULL;

// --- Estructuras de la UI ---

// Se actualiza para incluir una referencia a la ventana principal
typedef struct {
    GtkWindow* main_window;
    GtkTextBuffer *input_buffer;
    GtkTextBuffer *output_buffer;
} AppWidgets;


//=====================================================================
// LÓGICA PARA LA TABLA DE SÍMBOLOS
//=====================================================================

// Columnas que tendrá nuestra tabla
enum {
    COL_NAME,
    COL_TYPE,
    COL_SCOPE,
    NUM_COLS
};

// Función para crear y mostrar la ventana con la tabla de símbolos
static void display_symbol_table_window(GtkWindow* parent) {
    GtkWidget *window, *scrolled_window, *tree_view;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // 1. Crear la nueva ventana emergente
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tabla de Símbolos");
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 350);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent); // Se asocia a la ventana principal
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // 2. Crear el GtkListStore (el modelo de datos que contendrá la información)
    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

    // 3. Poblar el 'store' con los datos recolectados por el reportero
    const SymbolInfo* symbols = get_symbol_list();
    while(symbols) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COL_NAME, symbols->name,
                           COL_TYPE, labelTipoDato[symbols->type], // Usamos el array de result.h
                           COL_SCOPE, symbols->scope_level,
                           -1);
        symbols = symbols->next;
    }

    // 4. Crear el GtkTreeView (el widget visual que muestra la tabla)
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    // 5. Crear las columnas de la tabla y asociarlas a los datos del 'store'
    // Columna "Nombre"
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nombre", renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Columna "Tipo"
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Tipo", renderer, "text", COL_TYPE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    // Columna "Ámbito"
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Ámbito", renderer, "text", COL_SCOPE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    // 6. Añadir la tabla a una ventana con scroll y luego a la ventana principal
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    gtk_widget_show_all(window);
}

// Callback para el nuevo botón "Mostrar Tabla de Símbolos"
static void on_show_symbols_clicked(GtkButton* button, gpointer user_data) {
    (void)button; // Evitar warning de parámetro no usado
    AppWidgets* widgets = (AppWidgets*) user_data;
    
    // Obtener el texto de entrada
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(widgets->input_buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(widgets->input_buffer, &start, &end, FALSE);

    // Limpiar y re-analizar el código para poblar el reportero de símbolos
    clear_symbol_report();
    ast_root = NULL;

    if (input_text && input_text[0] != '\0') {
        YY_BUFFER_STATE buffer_state = yy_scan_string(input_text);
        if (yyparse() == 0 && ast_root) {
            Context* contextPadre = nuevoContext(NULL);
            // La interpretación llenará el reportero gracias al cambio en context.c
            ast_root->interpret(ast_root, contextPadre); 
        }
        yy_delete_buffer(buffer_state);
        // liberarAST(ast_root); // Deberías liberar el AST aquí
    }
    
    // Mostrar la ventana con la tabla ya poblada
    display_symbol_table_window(widgets->main_window);
    
    g_free(input_text);
}


//=====================================================================
// LÓGICA PRINCIPAL DE LA UI
//=====================================================================

// Función que se ejecuta al presionar el botón "Ejecutar"
static void on_execute_clicked(GtkButton* button, gpointer user_data) {
    (void)button; // Evitar warning de parámetro no usado
    AppWidgets* widgets = (AppWidgets*) user_data;

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(widgets->input_buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(widgets->input_buffer, &start, &end, FALSE);

    clear_output_buffer();
    ast_root = NULL;

    if (input_text && input_text[0] != '\0') {
        YY_BUFFER_STATE buffer_state = yy_scan_string(input_text);
        if (yyparse() == 0 && ast_root) {
            Context* contextPadre = nuevoContext(NULL);
            ast_root->interpret(ast_root, contextPadre);
        } else {
            append_to_output("Error de sintaxis o análisis.\n");
        }
        yy_delete_buffer(buffer_state);
        // liberarAST(ast_root); // Deberías liberar el AST aquí
    } else {
        append_to_output("No hay código para ejecutar.\n");
    }

    const char* output_text = get_output_buffer();
    gtk_text_buffer_set_text(widgets->output_buffer, output_text, -1);
    g_free(input_text);
}

// Función que construye la interfaz gráfica
static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window, *vbox, *hbox_buttons, *paned, *scrolled_input, *scrolled_output, *input_view, *output_view, *btn_exec, *btn_symbols;
    AppWidgets* widgets = (AppWidgets*) user_data;

    window = gtk_application_window_new(app);
    widgets->main_window = GTK_WINDOW(window); // Guardar referencia a la ventana principal
    gtk_window_set_title(GTK_WINDOW(window), "Mi Intérprete");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    // Lado Izquierdo: Entrada de Texto
    scrolled_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add1(GTK_PANED(paned), scrolled_input);
    gtk_widget_set_size_request(scrolled_input, 390, -1);
    
    input_view = gtk_text_view_new();
    widgets->input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input_view));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(input_view), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scrolled_input), input_view);
    
    // Lado Derecho: Salida de Texto
    scrolled_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add2(GTK_PANED(paned), scrolled_output);

    output_view = gtk_text_view_new();
    widgets->output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_output), output_view);

    // Caja horizontal para agrupar los botones
    hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 0);

    // Botón de Ejecutar
    btn_exec = gtk_button_new_with_label("Ejecutar");
    g_signal_connect(btn_exec, "clicked", G_CALLBACK(on_execute_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_exec, TRUE, TRUE, 0);

    // Botón de Tabla de Símbolos
    btn_symbols = gtk_button_new_with_label("Mostrar Tabla de Símbolos");
    g_signal_connect(btn_symbols, "clicked", G_CALLBACK(on_show_symbols_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_symbols, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
}

// Función principal del programa
int main(int argc, char** argv) {
    // Inicializar nuestros módulos
    init_output_buffer();
    init_symbol_report(); 

    // Inicializar GTK
    GtkApplication *app;
    int status;
    AppWidgets* widgets = g_malloc(sizeof(AppWidgets));

    app = gtk_application_new("com.tuinterprete.gtk", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), widgets);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // Liberar memoria al cerrar
    g_free(widgets);
    free_output_buffer();
    free_symbol_report(); 
    yylex_destroy();

    return status;
}
#include <gtk/gtk.h>
#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "output_buffer.h"
#include "symbol_reporter.h"
#include "error_reporter.h"

// --- Declaraciones y Prototipos (sin cambios) ---
typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern int yyparse(void);
extern int yylineno;
extern int yycolumn;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE b);
extern void yylex_destroy();

AbstractExpresion* ast_root = NULL;

typedef struct {
    GtkWindow* main_window;
    GtkTextBuffer *input_buffer;
    GtkTextBuffer *output_buffer;
} AppWidgets;

static void on_execute_clicked(GtkButton* button, gpointer user_data);
static void on_show_symbols_clicked(GtkButton* button, gpointer user_data);
static void on_show_errors_clicked(GtkButton* button, gpointer user_data);
static void display_error_table_window(GtkWindow* parent);
static void display_symbol_table_window(GtkWindow* parent);


// --- Lógica de Tablas (display_error_table_window y display_symbol_table_window sin cambios) ---
enum {
    ERR_COL_ID, ERR_COL_TYPE, ERR_COL_LEXEME, ERR_COL_DESC, ERR_COL_LINE, ERR_COL_COLUMN, ERR_NUM_COLS
};
static void display_error_table_window(GtkWindow* parent) {
    GtkWidget *window, *scrolled_window, *tree_view;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tabla de Errores");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    store = gtk_list_store_new(ERR_NUM_COLS, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT);
    const ErrorInfo* errors = get_error_list();
    while(errors) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           ERR_COL_ID, errors->id, ERR_COL_TYPE, errors->type,
                           ERR_COL_LEXEME, errors->lexeme, ERR_COL_DESC, errors->description,
                           ERR_COL_LINE, errors->line, ERR_COL_COLUMN, errors->column, -1);
        errors = errors->next;
    }
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("#", renderer, "text", ERR_COL_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Tipo", renderer, "text", ERR_COL_TYPE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Lexema", renderer, "text", ERR_COL_LEXEME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Descripción", renderer, "text", ERR_COL_DESC, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Línea", renderer, "text", ERR_COL_LINE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Columna", renderer, "text", ERR_COL_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    gtk_widget_show_all(window);
}

enum {
    SYM_COL_ID, SYM_COL_NAME, SYM_COL_TYPE, SYM_COL_VALUE,
    SYM_COL_CONTEXT, SYM_COL_LINE, SYM_COL_COLUMN, SYM_NUM_COLS
};
static void display_symbol_table_window(GtkWindow* parent) {
    GtkWidget *window, *scrolled_window, *tree_view;
    GtkListStore *store;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Tabla de Símbolos");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    store = gtk_list_store_new(SYM_NUM_COLS, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT);
    const SymbolInfo* symbols = get_symbol_list();
    while(symbols) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           SYM_COL_ID, symbols->id, SYM_COL_NAME, symbols->name,
                           SYM_COL_TYPE, symbols->type, SYM_COL_VALUE, symbols->value,
                           SYM_COL_CONTEXT, symbols->context_name, SYM_COL_LINE, symbols->line,
                           SYM_COL_COLUMN, symbols->column, -1);
        symbols = symbols->next;
    }
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("#", renderer, "text", SYM_COL_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nombre", renderer, "text", SYM_COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Tipo", renderer, "text", SYM_COL_TYPE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Valor", renderer, "text", SYM_COL_VALUE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Contexto", renderer, "text", SYM_COL_CONTEXT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Línea", renderer, "text", SYM_COL_LINE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Columna", renderer, "text", SYM_COL_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    gtk_widget_show_all(window);
}


// --- Función `on_execute_clicked` con la corrección ---
static void on_execute_clicked(GtkButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(widgets->input_buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(widgets->input_buffer, &start, &end, FALSE);

    clear_output_buffer();
    clear_symbol_report();
    clear_error_report();
    ast_root = NULL;
    yylineno = 1;
    yycolumn = 1;

    if (input_text && input_text[0] != '\0') {
        YY_BUFFER_STATE buffer_state = yy_scan_string(input_text);
        yyparse();
        
        if (get_error_list() == NULL && ast_root) {
            Context* contextPadre = nuevoContext(NULL, "global");
            ast_root->interpret(ast_root, contextPadre);

            // <<<<< CAMBIO CLAVE: Comprobar errores SEMÁNTICOS después de interpretar >>>>>
            if (has_semantic_error_been_found()) {
                append_to_output("\nSe encontró un error semántico. Ejecución abortada.");
                // Mostrar automáticamente la tabla de errores para que el usuario sepa qué pasó.
                display_error_table_window(widgets->main_window);
            }

        } else if (get_error_list() != NULL) {
            append_to_output("Se encontraron errores léxicos/sintácticos. La ejecución fue abortada.\n");
        } else {
             append_to_output("No se pudo generar el AST por un error desconocido.\n");
        }
        yy_delete_buffer(buffer_state);
        // liberarAST(ast_root);
    } else {
        append_to_output("No hay código para ejecutar.\n");
    }

    const char* output_text = get_output_buffer();
    gtk_text_buffer_set_text(widgets->output_buffer, output_text, -1);
    g_free(input_text);
}


// --- `on_show_symbols_clicked` y `on_show_errors_clicked` (sin cambios) ---
static void on_show_symbols_clicked(GtkButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(widgets->input_buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(widgets->input_buffer, &start, &end, FALSE);
    clear_symbol_report();
    clear_error_report();
    ast_root = NULL;
    yylineno = 1;
    yycolumn = 1;
    if (input_text && input_text[0] != '\0') {
        YY_BUFFER_STATE buffer_state = yy_scan_string(input_text);
        yyparse();
        if(get_error_list() == NULL && ast_root) {
            Context* contextPadre = nuevoContext(NULL, "global");
            ast_root->interpret(ast_root, contextPadre);
        }
        yy_delete_buffer(buffer_state);
    }
    display_symbol_table_window(widgets->main_window);
    g_free(input_text);
}

static void on_show_errors_clicked(GtkButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(widgets->input_buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(widgets->input_buffer, &start, &end, FALSE);
    clear_error_report();
    ast_root = NULL;
    yylineno = 1;
    yycolumn = 1;
    if (input_text && input_text[0] != '\0') {
        YY_BUFFER_STATE buffer_state = yy_scan_string(input_text);
        yyparse();
        yy_delete_buffer(buffer_state);
    }
    display_error_table_window(widgets->main_window);
    g_free(input_text);
}


// --- `activate` y `main` (sin cambios) ---
static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window, *vbox, *hbox_buttons, *paned, *scrolled_input, *scrolled_output, *input_view, *output_view, *btn_exec, *btn_symbols, *btn_errors;
    AppWidgets* widgets = (AppWidgets*) user_data;
    window = gtk_application_window_new(app);
    widgets->main_window = GTK_WINDOW(window);
    gtk_window_set_title(GTK_WINDOW(window), "JavaLang Interpreter");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);
    scrolled_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add1(GTK_PANED(paned), scrolled_input);
    input_view = gtk_text_view_new();
    widgets->input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input_view));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(input_view), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scrolled_input), input_view);
    scrolled_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_paned_add2(GTK_PANED(paned), scrolled_output);
    output_view = gtk_text_view_new();
    widgets->output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_output), output_view);
    hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 0);
    btn_exec = gtk_button_new_with_label("Ejecutar");
    g_signal_connect(btn_exec, "clicked", G_CALLBACK(on_execute_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_exec, TRUE, TRUE, 0);
    btn_symbols = gtk_button_new_with_label("Mostrar Tabla de Símbolos");
    g_signal_connect(btn_symbols, "clicked", G_CALLBACK(on_show_symbols_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_symbols, TRUE, TRUE, 0);
    btn_errors = gtk_button_new_with_label("Mostrar Errores");
    g_signal_connect(btn_errors, "clicked", G_CALLBACK(on_show_errors_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_errors, TRUE, TRUE, 0);
    gtk_widget_show_all(window);
}

int main(int argc, char** argv) {
    init_output_buffer();
    init_symbol_report();
    init_error_report();
    GtkApplication *app;
    int status;
    AppWidgets* widgets = g_malloc(sizeof(AppWidgets));
    app = gtk_application_new("com.mi.interprete", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), widgets);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    g_free(widgets);
    free_output_buffer();
    free_symbol_report(); 
    free_error_report();
    yylex_destroy();
    return status;
}

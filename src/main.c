#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast/AbstractExpresion.h"
#include "context/context.h"
#include "output_buffer.h"
#include "symbol_reporter.h"
#include "error_reporter.h"

// --- Declaraciones Globales y Prototipos ---
typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern int yyparse(void);
extern int yylineno;
extern int yycolumn;
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE b);
extern void yylex_destroy();

AbstractExpresion* ast_root = NULL;
static char* current_file_path = NULL;

typedef struct {
    GtkWindow* main_window;
    GtkSourceBuffer *input_buffer;
    GtkTextBuffer *output_buffer;
} AppWidgets;

// Prototipos
static void on_new_clicked(GtkToolButton* button, gpointer user_data);
static void on_open_clicked(GtkToolButton* button, gpointer user_data);
static void on_save_clicked(GtkToolButton* button, gpointer user_data);
static void on_save_as_clicked(GtkToolButton* button, gpointer user_data);
static void on_execute_clicked(GtkToolButton* button, gpointer user_data);
static void on_show_symbols_clicked(GtkToolButton* button, gpointer user_data);
static void on_show_errors_clicked(GtkToolButton* button, gpointer user_data);
static void on_generate_ast_clicked(GtkToolButton* button, gpointer user_data);

static void display_error_table_window(GtkWindow* parent);
static void display_symbol_table_window(GtkWindow* parent);
static gboolean save_file_content(AppWidgets* widgets, const char* path);
static void open_file(AppWidgets* widgets, const char* path);


//=====================================================================
// LÓGICA DE GESTIÓN DE ARCHIVOS
//=====================================================================
static void open_file(AppWidgets* widgets, const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        perror("fopen");
        return;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* content = malloc(length + 1);
    if (content) {
        fread(content, 1, length, file);
        content[length] = '\0';
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(widgets->input_buffer), content, -1);
        free(content);
    }
    fclose(file);

    if (current_file_path) {
        g_free(current_file_path);
    }
    current_file_path = g_strdup(path);
    char* basename = g_path_get_basename(path);
    char* window_title = g_strdup_printf("%s - JavaLang Interpreter", basename);
    gtk_window_set_title(widgets->main_window, window_title);
    g_free(basename);
    g_free(window_title);
}

static gboolean save_file_content(AppWidgets* widgets, const char* path) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(widgets->input_buffer), &start, &end);
    char* content = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(widgets->input_buffer), &start, &end, FALSE);
    FILE* file = fopen(path, "w");
    if (!file) {
        perror("fopen on save");
        g_free(content);
        return FALSE;
    }
    fprintf(file, "%s", content);
    fclose(file);
    g_free(content);

    if (current_file_path) {
        g_free(current_file_path);
    }
    current_file_path = g_strdup(path);
    char* basename = g_path_get_basename(path);
    char* window_title = g_strdup_printf("%s - JavaLang Interpreter", basename);
    gtk_window_set_title(widgets->main_window, window_title);
    g_free(basename);
    g_free(window_title);
    return TRUE;
}

//=====================================================================
// DEFINICIONES DE LAS FUNCIONES CALLBACK
//=====================================================================

static void on_new_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(widgets->input_buffer), "", -1);
    if (current_file_path) {
        g_free(current_file_path);
        current_file_path = NULL;
    }
    gtk_window_set_title(widgets->main_window, "JavaLang Interpreter");
}

static void on_open_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Abrir Archivo", widgets->main_window, GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancelar", GTK_RESPONSE_CANCEL,
                                         "_Abrir", GTK_RESPONSE_ACCEPT, NULL);
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Archivos JavaLang (*.usl)");
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        open_file(widgets, filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_save_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    if (current_file_path == NULL) {
        on_save_as_clicked(button, user_data);
    } else {
        save_file_content((AppWidgets*)user_data, current_file_path);
    }
}

static void on_save_as_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Guardar Archivo Como", widgets->main_window, GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancelar", GTK_RESPONSE_CANCEL,
                                         "_Guardar", GTK_RESPONSE_ACCEPT, NULL);
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Archivos JavaLang (*.usl)");
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "nuevo.usl");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        save_file_content(widgets, filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_execute_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    
    GtkTextBuffer* buffer = GTK_TEXT_BUFFER(widgets->input_buffer);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

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
            if (has_semantic_error_been_found()) {
                append_to_output("\nSe encontró un error semántico. Ejecución abortada.");
                display_error_table_window(widgets->main_window);
            } else {
                append_to_output("\n\n======== Código analizado y ejecutado correctamente ========");
            }
        } else if (get_error_list() != NULL) {
            append_to_output("Se encontraron errores léxicos/sintácticos. La ejecución fue abortada.\n");
        } else {
             append_to_output("No se pudo generar el AST por un error desconocido.\n");
        }
        yy_delete_buffer(buffer_state);
    } else {
        append_to_output("No hay código para ejecutar.\n");
    }

    const char* output_text = get_output_buffer();
    gtk_text_buffer_set_text(widgets->output_buffer, output_text, -1);
    g_free(input_text);
}

static void on_show_symbols_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    GtkTextBuffer* buffer = GTK_TEXT_BUFFER(widgets->input_buffer);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
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

static void on_show_errors_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    GtkTextBuffer* buffer = GTK_TEXT_BUFFER(widgets->input_buffer);
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar* input_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
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
    display_error_table_window(widgets->main_window);
    g_free(input_text);
}

static void on_generate_ast_clicked(GtkToolButton* button, gpointer user_data) {
    (void)button;
    AppWidgets* widgets = (AppWidgets*) user_data;
    gtk_text_buffer_set_text(widgets->output_buffer, "Funcionalidad 'Generar AST' no implementada todavía.", -1);
}

//=====================================================================
// CONSTRUCCIÓN DE LA INTERFAZ GRÁFICA
//=====================================================================
static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window, *vbox, *toolbar, *paned, *scrolled_input, *scrolled_output, *input_view, *output_view;
    GtkToolItem *new_tool, *open_tool, *save_tool, *save_as_tool, *spacer, *exec_tool, *ast_tool, *symbols_tool, *errors_tool;
    AppWidgets* widgets = (AppWidgets*) user_data;

    window = gtk_application_window_new(app);
    widgets->main_window = GTK_WINDOW(window);
    gtk_window_set_title(GTK_WINDOW(window), "JavaLang Interpreter");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 768);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "toolbar .execute-button button { background-image: image(green); color: white; font-weight: bold; }"
        "toolbar .analysis-button button { background-image: image(lightgreen); }",
        -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
    
    toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    new_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_SMALL_TOOLBAR), "Nuevo");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_tool, -1);
    
    open_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_SMALL_TOOLBAR), "Abrir");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_tool, -1);
    
    save_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_SMALL_TOOLBAR), "Guardar");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_tool, -1);
    
    save_as_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_SMALL_TOOLBAR), "Guardar Como");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_as_tool, -1);

    spacer = gtk_separator_tool_item_new();
    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(spacer), FALSE);
    gtk_tool_item_set_expand(spacer, TRUE);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), spacer, -1);

    errors_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("dialog-error", GTK_ICON_SIZE_SMALL_TOOLBAR), "Tabla Errores");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), errors_tool, -1);
    
    symbols_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("view-list-text", GTK_ICON_SIZE_SMALL_TOOLBAR), "Tabla Símbolos");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), symbols_tool, -1);
    
    ast_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("preferences-system", GTK_ICON_SIZE_SMALL_TOOLBAR), "Generar AST");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ast_tool, -1);

    exec_tool = gtk_tool_button_new(gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_SMALL_TOOLBAR), "Ejecutar");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), exec_tool, -1);

    gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(exec_tool)), "execute-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(errors_tool)), "analysis-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(symbols_tool)), "analysis-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(ast_tool)), "analysis-button");

    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);

    // Contenedor para la entrada
    GtkWidget* left_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget* input_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(input_label), "<span weight='bold' size='large'>Entrada del Programa</span>");
    gtk_widget_set_halign(input_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(left_vbox), input_label, FALSE, FALSE, 5);

    scrolled_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_input, TRUE);
    gtk_box_pack_start(GTK_BOX(left_vbox), scrolled_input, TRUE, TRUE, 0);
    gtk_paned_add1(GTK_PANED(paned), left_vbox);

    input_view = gtk_source_view_new();
    widgets->input_buffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(input_view)));
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(input_view), GTK_WRAP_WORD_CHAR);
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(input_view), TRUE);
    gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(input_view), TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_input), input_view);
    
    GtkSourceLanguageManager* lang_manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage* lang = gtk_source_language_manager_get_language(lang_manager, "java");
    gtk_source_buffer_set_language(widgets->input_buffer, lang);

    // Contenedor para la salida
    GtkWidget* right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget* output_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(output_label), "<span weight='bold' size='large'>Salida del Programa</span>");
    gtk_widget_set_halign(output_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(right_vbox), output_label, FALSE, FALSE, 5);

    scrolled_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_output, TRUE);
    gtk_box_pack_start(GTK_BOX(right_vbox), scrolled_output, TRUE, TRUE, 0);
    gtk_paned_add2(GTK_PANED(paned), right_vbox);

    output_view = gtk_text_view_new();
    widgets->output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_output), output_view);

    gtk_paned_set_position(GTK_PANED(paned), 600);

    g_signal_connect(G_OBJECT(new_tool), "clicked", G_CALLBACK(on_new_clicked), widgets);
    g_signal_connect(G_OBJECT(open_tool), "clicked", G_CALLBACK(on_open_clicked), widgets);
    g_signal_connect(G_OBJECT(save_tool), "clicked", G_CALLBACK(on_save_clicked), widgets);
    g_signal_connect(G_OBJECT(save_as_tool), "clicked", G_CALLBACK(on_save_as_clicked), widgets);
    g_signal_connect(G_OBJECT(exec_tool), "clicked", G_CALLBACK(on_execute_clicked), widgets);
    g_signal_connect(G_OBJECT(ast_tool), "clicked", G_CALLBACK(on_generate_ast_clicked), widgets);
    g_signal_connect(G_OBJECT(symbols_tool), "clicked", G_CALLBACK(on_show_symbols_clicked), widgets);
    g_signal_connect(G_OBJECT(errors_tool), "clicked", G_CALLBACK(on_show_errors_clicked), widgets);

    gtk_widget_show_all(window);
}

// --- Función Principal ---
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
    
    if (current_file_path) {
        g_free(current_file_path);
    }

    return status;
}

//=====================================================================
// LÓGICA DE TABLAS (SIN CAMBIOS)
//=====================================================================
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


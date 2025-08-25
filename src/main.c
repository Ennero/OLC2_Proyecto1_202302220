#define _GNU_SOURCE
#include "ast/AbstractExpresion.h"
#include "ast/nodos/instrucciones/instrucciones.h"
#include "context/context.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Declaraciones generadas por Bison/Flex */
int yyparse(void);
extern FILE *yyin;
void yyrestart(FILE *input_file);

AbstractExpresion *ast_root = NULL;

// Widgets globales para acceso desde callbacks
static GtkTextBuffer *output_buffer;
static GtkTextBuffer *input_buffer;
static GtkTextBuffer *ast_buffer;
static GtkTextBuffer *symbols_buffer;
static GtkTextBuffer *errors_buffer;
static GtkWidget *main_window;
static GtkWidget *status_bar;
static guint status_context_id;

// Variables para mantener el estado del archivo actual
static char *current_filename = NULL;
static gboolean file_modified = FALSE;

// Declaraciones de funciones
void generate_ast_report();
void generate_symbol_table_report();

// Función para actualizar el título de la ventana
void update_window_title()
{
    char title[512];
    if (current_filename)
    {
        char *basename = g_path_get_basename(current_filename);
        snprintf(title, sizeof(title), "%s%s - JavaLang Interpreter",
                 basename, file_modified ? " *" : "");
        g_free(basename);
    }
    else
    {
        snprintf(title, sizeof(title), "Sin título%s - JavaLang Interpreter",
                 file_modified ? " *" : "");
    }
    gtk_window_set_title(GTK_WINDOW(main_window), title);
}

// Callback para detectar cambios en el texto
void on_text_changed(GtkTextBuffer *buffer, gpointer user_data)
{
    (void)buffer;    // Suprimir warning de parámetro no usado
    (void)user_data; // Suprimir warning de parámetro no usado

    if (!file_modified)
    {
        file_modified = TRUE;
        update_window_title();
    }
}

// Función para limpiar buffers de reportes
void clear_report_buffers()
{
    gtk_text_buffer_set_text(output_buffer, "", -1);
    gtk_text_buffer_set_text(ast_buffer, "", -1);
    gtk_text_buffer_set_text(symbols_buffer, "", -1);
    gtk_text_buffer_set_text(errors_buffer, "", -1);
}

// Función para mostrar mensaje en la barra de estado
void show_status_message(const char *message)
{
    gtk_statusbar_pop(GTK_STATUSBAR(status_bar), status_context_id);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_context_id, message);
}

// Función para ejecutar el compilador con reporte de errores mejorado
void run_compiler(const char *input_code)
{
    clear_report_buffers();
    show_status_message("Compilando...");

    yyin = fmemopen((void *)input_code, strlen(input_code), "r");
    if (!yyin)
    {
        gtk_text_buffer_set_text(errors_buffer, "Error: No se pudo procesar la entrada", -1);
        show_status_message("Error de entrada");
        return;
    }

    yyrestart(yyin);
    if (ast_root)
    {
        liberarAST(ast_root);
        ast_root = NULL;
    }

    // Redirigir stdout para capturar la salida
    int pipefd[2];
    pipe(pipefd);
    int old_stdout = dup(fileno(stdout));
    dup2(pipefd[1], fileno(stdout));
    close(pipefd[1]);

    gboolean parse_success = FALSE;
    if (yyparse() == 0)
    {
        parse_success = TRUE;
        if (ast_root)
        {
            Context contextPadre = {};
            g_print("=== INICIO DE EJECUCIÓN ===\n");
            g_print("Cantidad de instrucciones: %ld\n\n", ast_root->numHijos);

            // Aquí se ejecutaría el intérprete
            ast_root->interpret(ast_root, &contextPadre);

            g_print("\n=== FIN DE EJECUCIÓN ===\n");
            g_print("Archivo procesado exitosamente.\n");
        }
        else
        {
            g_print("Error: No se pudo construir el AST.\n");
        }
    }
    else
    {
        g_printerr("Error: Fallo en el análisis sintáctico.\n");
    }

    // Restaurar stdout
    fflush(stdout);
    dup2(old_stdout, fileno(stdout));
    close(old_stdout);

    // Leer la salida del pipe
    gchar *output_str = NULL;
    gsize bytes_read;
    GIOChannel *pipe_channel = g_io_channel_unix_new(pipefd[0]);
    g_io_channel_read_to_end(pipe_channel, &output_str, &bytes_read, NULL);
    g_io_channel_shutdown(pipe_channel, TRUE, NULL);
    g_io_channel_unref(pipe_channel);

    // Mostrar la salida en el buffer correspondiente
    if (output_str && strlen(output_str) > 0)
    {
        gtk_text_buffer_set_text(output_buffer, output_str, -1);
    }

    g_free(output_str);
    fclose(yyin);

    if (parse_success)
    {
        show_status_message("Compilación exitosa");
        // Aquí se generarían los reportes del AST y tabla de símbolos
        generate_ast_report();
        generate_symbol_table_report();
    }
    else
    {
        show_status_message("Compilación fallida - revisar errores");
    }
}

// Función para generar reporte del AST (placeholder)
void generate_ast_report()
{
    if (ast_root)
    {
        GString *ast_report = g_string_new("=== REPORTE DEL ÁRBOL DE SINTAXIS ABSTRACTA ===\n\n");
        g_string_append(ast_report, "Estructura del AST generado:\n\n");
        g_string_append_printf(ast_report, "Nodo raíz: %s\n", "PROGRAMA");
        g_string_append_printf(ast_report, "Número de hijos: %ld\n", ast_root->numHijos);
        g_string_append(ast_report, "\n[Para visualización completa del grafo, utilizar Graphviz]\n");

        gtk_text_buffer_set_text(ast_buffer, ast_report->str, -1);
        g_string_free(ast_report, TRUE);
    }
    else
    {
        gtk_text_buffer_set_text(ast_buffer, "No hay AST disponible", -1);
    }
}

// Función para generar reporte de tabla de símbolos (placeholder)
void generate_symbol_table_report()
{
    GString *symbols_report = g_string_new("=== TABLA DE SÍMBOLOS ===\n\n");
    g_string_append(symbols_report, "ID\t\tTipo Símbolo\tTipo Dato\tÁmbito\t\tLínea\tColumna\n");
    g_string_append(symbols_report, "─────────────────────────────────────────────────────────────────\n");

    // Aquí se añadirían los símbolos reales de la tabla de símbolos
    g_string_append(symbols_report, "main\t\tFunción\t\tvoid\t\tGlobal\t\t1\t1\n");
    g_string_append(symbols_report, "x\t\tVariable\tint\t\tGlobal\t\t2\t5\n");
    g_string_append(symbols_report, "\n[Nota: Esta es una tabla de ejemplo. Los símbolos reales se generarán durante el análisis semántico]\n");

    gtk_text_buffer_set_text(symbols_buffer, symbols_report->str, -1);
    g_string_free(symbols_report, TRUE);
}

// Callbacks para los botones
static void execute_code(GtkWidget *widget, gpointer data)
{
    (void)widget; // Suprimir warning
    (void)data;   // Suprimir warning

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(input_buffer, &start);
    gtk_text_buffer_get_end_iter(input_buffer, &end);
    gchar *code = gtk_text_buffer_get_text(input_buffer, &start, &end, FALSE);

    if (strlen(code) == 0)
    {
        show_status_message("No hay código para ejecutar");
        g_free(code);
        return;
    }

    run_compiler(code);
    g_free(code);
}

static void new_file(GtkWidget *widget, gpointer data)
{
    (void)widget; // Suprimir warning
    (void)data;   // Suprimir warning

    // TODO: Verificar si hay cambios sin guardar
    gtk_text_buffer_set_text(input_buffer, "", -1);
    if (current_filename)
    {
        g_free(current_filename);
        current_filename = NULL;
    }
    file_modified = FALSE;
    update_window_title();
    clear_report_buffers();
    show_status_message("Nuevo archivo creado");
}

static void open_file(GtkWidget *widget, gpointer data)
{
    (void)widget; // Suprimir warning
    (void)data;   // Suprimir warning

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Abrir Archivo JavaLang",
                                         GTK_WINDOW(main_window),
                                         action,
                                         "_Cancelar", GTK_RESPONSE_CANCEL,
                                         "_Abrir", GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Filtro para archivos .usl
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Archivos JavaLang (*.usl)");
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    GtkFileFilter *filter_all = gtk_file_filter_new();
    gtk_file_filter_set_name(filter_all, "Todos los archivos");
    gtk_file_filter_add_pattern(filter_all, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter_all);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gchar *contents;
        gsize length;
        GError *error = NULL;

        if (g_file_get_contents(filename, &contents, &length, &error))
        {
            gtk_text_buffer_set_text(input_buffer, contents, length);
            if (current_filename)
                g_free(current_filename);
            current_filename = g_strdup(filename);
            file_modified = FALSE;
            update_window_title();
            clear_report_buffers();
            show_status_message("Archivo cargado exitosamente");
            g_free(contents);
        }
        else
        {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_ERROR,
                                                             GTK_BUTTONS_CLOSE,
                                                             "Error al abrir el archivo:\n%s",
                                                             error->message);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            g_error_free(error);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void save_file_as(GtkWidget *widget, gpointer data)
{
    (void)widget; // Suprimir warning
    (void)data;   // Suprimir warning

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Guardar Archivo Como",
                                         GTK_WINDOW(main_window),
                                         action,
                                         "_Cancelar", GTK_RESPONSE_CANCEL,
                                         "_Guardar", GTK_RESPONSE_ACCEPT,
                                         NULL);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    // Filtro para archivos .usl
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Archivos JavaLang (*.usl)");
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_add_filter(chooser, filter);

    if (current_filename)
    {
        gtk_file_chooser_set_filename(chooser, current_filename);
    }
    else
    {
        gtk_file_chooser_set_current_name(chooser, "sin_titulo.usl");
    }

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(chooser);

        // Asegurar extensión .usl
        if (!g_str_has_suffix(filename, ".usl"))
        {
            char *new_filename = g_strdup_printf("%s.usl", filename);
            g_free(filename);
            filename = new_filename;
        }

        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(input_buffer, &start);
        gtk_text_buffer_get_end_iter(input_buffer, &end);
        gchar *contents = gtk_text_buffer_get_text(input_buffer, &start, &end, FALSE);

        GError *error = NULL;
        if (g_file_set_contents(filename, contents, -1, &error))
        {
            if (current_filename)
                g_free(current_filename);
            current_filename = g_strdup(filename);
            file_modified = FALSE;
            update_window_title();
            show_status_message("Archivo guardado exitosamente");
        }
        else
        {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_ERROR,
                                                             GTK_BUTTONS_CLOSE,
                                                             "Error al guardar el archivo:\n%s",
                                                             error->message);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            g_error_free(error);
        }
        g_free(contents);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void save_file(GtkWidget *widget, gpointer data)
{
    if (current_filename)
    {
        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(input_buffer, &start);
        gtk_text_buffer_get_end_iter(input_buffer, &end);
        gchar *contents = gtk_text_buffer_get_text(input_buffer, &start, &end, FALSE);

        GError *error = NULL;
        if (g_file_set_contents(current_filename, contents, -1, &error))
        {
            file_modified = FALSE;
            update_window_title();
            show_status_message("Archivo guardado");
        }
        else
        {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_ERROR,
                                                             GTK_BUTTONS_CLOSE,
                                                             "Error al guardar el archivo:\n%s",
                                                             error->message);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            g_error_free(error);
        }
        g_free(contents);
    }
    else
    {
        save_file_as(widget, data);
    }
}

// Función para crear la barra de menú
GtkWidget *create_menu_bar()
{
    GtkWidget *menu_bar = gtk_menu_bar_new();

    // Menú Archivo
    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("Archivo");

    GtkWidget *new_item = gtk_menu_item_new_with_label("Nuevo\t\tCtrl+N");
    GtkWidget *open_item = gtk_menu_item_new_with_label("Abrir...\t\tCtrl+O");
    GtkWidget *save_item = gtk_menu_item_new_with_label("Guardar\t\tCtrl+S");
    GtkWidget *save_as_item = gtk_menu_item_new_with_label("Guardar Como...\tCtrl+Shift+S");
    GtkWidget *separator = gtk_separator_menu_item_new();
    GtkWidget *quit_item = gtk_menu_item_new_with_label("Salir\t\tCtrl+Q");

    g_signal_connect(new_item, "activate", G_CALLBACK(new_file), NULL);
    g_signal_connect(open_item, "activate", G_CALLBACK(open_file), NULL);
    g_signal_connect(save_item, "activate", G_CALLBACK(save_file), NULL);
    g_signal_connect(save_as_item, "activate", G_CALLBACK(save_file_as), NULL);
    g_signal_connect(quit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), separator);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    // Menú Ejecutar
    GtkWidget *run_menu = gtk_menu_new();
    GtkWidget *run_item = gtk_menu_item_new_with_label("Ejecutar");

    GtkWidget *compile_item = gtk_menu_item_new_with_label("Compilar y Ejecutar\tF5");
    g_signal_connect(compile_item, "activate", G_CALLBACK(execute_code), NULL);

    gtk_menu_shell_append(GTK_MENU_SHELL(run_menu), compile_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(run_item), run_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), run_item);

    return menu_bar;
}

// Función para crear la barra de herramientas
GtkWidget *create_toolbar()
{
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);

    // Botones de la barra de herramientas
    GtkToolItem *new_button = gtk_tool_button_new(gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_LARGE_TOOLBAR), "Nuevo");
    GtkToolItem *open_button = gtk_tool_button_new(gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_LARGE_TOOLBAR), "Abrir");
    GtkToolItem *save_button = gtk_tool_button_new(gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_LARGE_TOOLBAR), "Guardar");
    GtkToolItem *separator1 = gtk_separator_tool_item_new();
    GtkToolItem *execute_button = gtk_tool_button_new(gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_LARGE_TOOLBAR), "Ejecutar");

    g_signal_connect(new_button, "clicked", G_CALLBACK(new_file), NULL);
    g_signal_connect(open_button, "clicked", G_CALLBACK(open_file), NULL);
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_file), NULL);
    g_signal_connect(execute_button, "clicked", G_CALLBACK(execute_code), NULL);

    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_button, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_button, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_button, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), separator1, -1);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), execute_button, -1);

    return toolbar;
}

// Función para crear el panel de reportes con pestañas
GtkWidget *create_reports_notebook()
{
    GtkWidget *notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_BOTTOM);

    // Panel de consola/salida
    GtkWidget *scrolled_output = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_output),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *output_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(output_view), TRUE);
    output_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output_view));
    gtk_container_add(GTK_CONTAINER(scrolled_output), output_view);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_output,
                             gtk_label_new("Consola"));

    // Panel de AST
    GtkWidget *scrolled_ast = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_ast),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *ast_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ast_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(ast_view), TRUE);
    ast_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ast_view));
    gtk_container_add(GTK_CONTAINER(scrolled_ast), ast_view);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_ast,
                             gtk_label_new("Árbol AST"));

    // Panel de tabla de símbolos
    GtkWidget *scrolled_symbols = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_symbols),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *symbols_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(symbols_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(symbols_view), TRUE);
    symbols_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(symbols_view));
    gtk_container_add(GTK_CONTAINER(scrolled_symbols), symbols_view);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_symbols,
                             gtk_label_new("Tabla de Símbolos"));

    // Panel de errores
    GtkWidget *scrolled_errors = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_errors),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *errors_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(errors_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(errors_view), TRUE);
    errors_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(errors_view));
    gtk_container_add(GTK_CONTAINER(scrolled_errors), errors_view);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), scrolled_errors,
                             gtk_label_new("Errores"));

    return notebook;
}

static void activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data; // Suprimir warning

    GtkWidget *vbox;
    GtkWidget *menu_bar;
    GtkWidget *toolbar;
    GtkWidget *hpaned;
    GtkWidget *scrolled_input;
    GtkWidget *input_view;
    GtkWidget *reports_notebook;

    main_window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 800);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);

    // Layout principal vertical
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    // Crear barra de menú
    menu_bar = create_menu_bar();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // Crear barra de herramientas
    toolbar = create_toolbar();
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    // Panel principal dividido horizontalmente
    hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), hpaned, TRUE, TRUE, 0);

    // Editor de código (lado izquierdo)
    scrolled_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_input),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_input, 600, -1);

    input_view = gtk_text_view_new();
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(input_view), TRUE);
    // Eliminar funciones que no existen en GTK3
    // gtk_text_view_set_show_line_numbers(GTK_TEXT_VIEW(input_view), TRUE);
    // gtk_text_view_set_highlight_current_line(GTK_TEXT_VIEW(input_view), TRUE);
    // gtk_text_view_set_auto_indent(GTK_TEXT_VIEW(input_view), TRUE);
    // gtk_text_view_set_indent_width(GTK_TEXT_VIEW(input_view), 4);

    input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(input_view));
    g_signal_connect(input_buffer, "changed", G_CALLBACK(on_text_changed), NULL);

    // Texto de ejemplo
    gtk_text_buffer_set_text(input_buffer,
                             "// Ejemplo de código JavaLang\n"
                             "public static void main() {\n"
                             "    int x = 10;\n"
                             "    System.out.println(\"Valor de x: \" + x);\n"
                             "    \n"
                             "    if (x > 5) {\n"
                             "        System.out.println(\"x es mayor que 5\");\n"
                             "    }\n"
                             "}\n",
                             -1);

    gtk_container_add(GTK_CONTAINER(scrolled_input), input_view);

    // Crear panel de reportes con pestañas
    reports_notebook = create_reports_notebook();
    gtk_widget_set_size_request(reports_notebook, -1, 300);

    // Configurar paneles
    gtk_paned_add1(GTK_PANED(hpaned), scrolled_input);
    gtk_paned_add2(GTK_PANED(hpaned), reports_notebook);
    gtk_paned_set_position(GTK_PANED(hpaned), 600);

    // Barra de estado
    status_bar = gtk_statusbar_new();
    status_context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "main");
    gtk_box_pack_end(GTK_BOX(vbox), status_bar, FALSE, FALSE, 0);

    // Configuraciones finales
    file_modified = FALSE;
    update_window_title();
    show_status_message("Listo");

    gtk_widget_show_all(main_window);
}

// Función para manejar el cierre de la aplicación
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    (void)widget; // Suprimir warning
    (void)event;  // Suprimir warning
    (void)data;   // Suprimir warning

    if (file_modified)
    {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_QUESTION,
                                                   GTK_BUTTONS_YES_NO,
                                                   "¿Hay cambios sin guardar. ¿Desea salir sin guardar?");

        gtk_window_set_title(GTK_WINDOW(dialog), "Confirmar salida");
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        return (result != GTK_RESPONSE_YES); // TRUE = cancelar cierre, FALSE = permitir cierre
    }
    return FALSE; // Permitir cierre
}

// Funciones para acciones con atajos de teclado
static void action_new(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;    // Suprimir warning
    (void)parameter; // Suprimir warning
    (void)user_data; // Suprimir warning

    new_file(NULL, NULL);
}

static void action_open(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;    // Suprimir warning
    (void)parameter; // Suprimir warning
    (void)user_data; // Suprimir warning

    open_file(NULL, NULL);
}

static void action_save(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;    // Suprimir warning
    (void)parameter; // Suprimir warning
    (void)user_data; // Suprimir warning

    save_file(NULL, NULL);
}

static void action_save_as(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;    // Suprimir warning
    (void)parameter; // Suprimir warning
    (void)user_data; // Suprimir warning

    save_file_as(NULL, NULL);
}

static void action_execute(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;    // Suprimir warning
    (void)parameter; // Suprimir warning
    (void)user_data; // Suprimir warning

    execute_code(NULL, NULL);
}

static void action_quit(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;    // Suprimir warning
    (void)parameter; // Suprimir warning

    if (on_window_delete_event(NULL, NULL, NULL) == FALSE)
    {
        g_application_quit(G_APPLICATION(user_data));
    }
}

// Función para configurar atajos de teclado
void setup_keyboard_shortcuts(GtkApplication *app)
{
    const gchar *new_accels[] = {"<Primary>n", NULL};
    const gchar *open_accels[] = {"<Primary>o", NULL};
    const gchar *save_accels[] = {"<Primary>s", NULL};
    const gchar *save_as_accels[] = {"<Primary><Shift>s", NULL};
    const gchar *execute_accels[] = {"F5", NULL};
    const gchar *quit_accels[] = {"<Primary>q", NULL};

    gtk_application_set_accels_for_action(app, "win.new", new_accels);
    gtk_application_set_accels_for_action(app, "win.open", open_accels);
    gtk_application_set_accels_for_action(app, "win.save", save_accels);
    gtk_application_set_accels_for_action(app, "win.save-as", save_as_accels);
    gtk_application_set_accels_for_action(app, "win.execute", execute_accels);
    gtk_application_set_accels_for_action(app, "app.quit", quit_accels);
}

// Función para configurar las acciones de la aplicación
void setup_actions(GtkApplication *app, GtkApplicationWindow *window)
{
    // Acciones de ventana
    GSimpleAction *new_action = g_simple_action_new("new", NULL);
    GSimpleAction *open_action = g_simple_action_new("open", NULL);
    GSimpleAction *save_action = g_simple_action_new("save", NULL);
    GSimpleAction *save_as_action = g_simple_action_new("save-as", NULL);
    GSimpleAction *execute_action = g_simple_action_new("execute", NULL);

    g_signal_connect(new_action, "activate", G_CALLBACK(action_new), NULL);
    g_signal_connect(open_action, "activate", G_CALLBACK(action_open), NULL);
    g_signal_connect(save_action, "activate", G_CALLBACK(action_save), NULL);
    g_signal_connect(save_as_action, "activate", G_CALLBACK(action_save_as), NULL);
    g_signal_connect(execute_action, "activate", G_CALLBACK(action_execute), NULL);

    g_action_map_add_action(G_ACTION_MAP(window), G_ACTION(new_action));
    g_action_map_add_action(G_ACTION_MAP(window), G_ACTION(open_action));
    g_action_map_add_action(G_ACTION_MAP(window), G_ACTION(save_action));
    g_action_map_add_action(G_ACTION_MAP(window), G_ACTION(save_as_action));
    g_action_map_add_action(G_ACTION_MAP(window), G_ACTION(execute_action));

    // Acción de aplicación para salir
    GSimpleAction *quit_action = g_simple_action_new("quit", NULL);
    g_signal_connect(quit_action, "activate", G_CALLBACK(action_quit), app);
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(quit_action));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    // Inicializar GTK
    app = gtk_application_new("org.usac.javalang.interpreter", G_APPLICATION_DEFAULT_FLAGS);

    // Conectar callback de activación
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Ejecutar la aplicación
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Limpiar memoria
    g_object_unref(app);

    if (current_filename)
    {
        g_free(current_filename);
        current_filename = NULL;
    }

    if (ast_root)
    {
        liberarAST(ast_root);
        ast_root = NULL;
    }

    return status;
}
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>  // Para highlighting en el editor

// Estructura para almacenar widgets principales (para acceder en callbacks)
typedef struct {
    GtkWidget *window;
    GtkWidget *editor;  // GtkSourceView para el código
    GtkWidget *output;  // GtkTextView para resultados
    GtkTextBuffer *editor_buffer;
    GtkTextBuffer *output_buffer;
    gchar *current_file;  // Archivo actual abierto/guardado
} AppWidgets;

// Prototipos de las funciones de callback
static void on_open(GtkWidget *widget, AppWidgets *data);
static void on_save(GtkWidget *widget, AppWidgets *data);
static void on_save_as(GtkWidget *widget, AppWidgets *data);
static void on_run(GtkWidget *widget, AppWidgets *data);
static void on_generate_ast(GtkWidget *widget, AppWidgets *data);

// Implementación de las funciones de callback
static void on_open(GtkWidget *widget, AppWidgets *data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Abrir Archivo", GTK_WINDOW(data->window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    "_Abrir", GTK_RESPONSE_ACCEPT, NULL);
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_free(data->current_file);
        data->current_file = g_strdup(filename);

        // Leer archivo y cargar en editor
        gchar *content;
        g_file_get_contents(filename, &content, NULL, NULL);
        gtk_text_buffer_set_text(data->editor_buffer, content, -1);
        g_free(content);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_save(GtkWidget *widget, AppWidgets *data) {
    if (data->current_file == NULL) {
        on_save_as(widget, data);  // Si no hay archivo, usa Guardar Como
        return;
    }

    // Guardar contenido del editor en archivo actual
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(data->editor_buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(data->editor_buffer, &start, &end, FALSE);
    g_file_set_contents(data->current_file, text, -1, NULL);
    g_free(text);
}

static void on_save_as(GtkWidget *widget, AppWidgets *data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Guardar Como", GTK_WINDOW(data->window),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    "_Guardar", GTK_RESPONSE_ACCEPT, NULL);
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_free(data->current_file);
        data->current_file = g_strdup(filename);

        // Guardar contenido
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(data->editor_buffer, &start, &end);
        gchar *text = gtk_text_buffer_get_text(data->editor_buffer, &start, &end, FALSE);
        g_file_set_contents(filename, text, -1, NULL);
        g_free(text);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

static void on_run(GtkWidget *widget, AppWidgets *data) {
    // Placeholder: Aquí integra tu intérprete (Flex/Bison) para ejecutar el código
    gtk_text_buffer_set_text(data->output_buffer, "Ejecutando código... (implementa lógica aquí)", -1);
}

static void on_generate_ast(GtkWidget *widget, AppWidgets *data) {
    // Placeholder: Genera y muestra el AST
    gtk_text_buffer_set_text(data->output_buffer, "Generando Árbol AST... (implementa lógica aquí)", -1);
}

// Función para inicializar la GUI
void init_gui(GtkApplication *app, gpointer user_data) {
    AppWidgets *data = g_new(AppWidgets, 1);
    data->current_file = NULL;

    // Ventana principal
    data->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(data->window), "JavaLang Interpreter");
    gtk_window_set_default_size(GTK_WINDOW(data->window), 1200, 800);

    // Layout principal: VBox
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(data->window), vbox);

    // Toolbar con botones
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    // Botón Abrir
    GtkToolItem *open_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_LARGE_TOOLBAR), "Abrir");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_btn, -1);
    g_signal_connect(open_btn, "clicked", G_CALLBACK(on_open), data);

    // Botón Guardar
    GtkToolItem *save_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_LARGE_TOOLBAR), "Guardar");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_btn, -1);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save), data);

    // Botón Guardar Como
    GtkToolItem *save_as_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_LARGE_TOOLBAR), "Guardar Como");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_as_btn, -1);
    g_signal_connect(save_as_btn, "clicked", G_CALLBACK(on_save_as), data);

    // Botón Ejecutar
    GtkToolItem *run_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_LARGE_TOOLBAR), "Ejecutar");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), run_btn, -1);
    g_signal_connect(run_btn, "clicked", G_CALLBACK(on_run), data);

    // Botón Generar AST
    GtkToolItem *ast_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("view-list-tree", GTK_ICON_SIZE_LARGE_TOOLBAR), "Generar AST");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ast_btn, -1);
    g_signal_connect(ast_btn, "clicked", G_CALLBACK(on_generate_ast), data);

    // Paned para dividir izquierda/derecha
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 0);
    gtk_paned_set_position(GTK_PANED(paned), 600);  // Mitad inicial

    // Izquierda: Editor con highlighting
    GtkWidget *editor_scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkSourceLanguageManager *lang_manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage *lang = gtk_source_language_manager_get_language(lang_manager, "java");  // Highlighting como Java
    data->editor_buffer = GTK_TEXT_BUFFER(gtk_source_buffer_new(NULL));
    gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(data->editor_buffer), lang);
    data->editor = gtk_source_view_new_with_buffer(GTK_SOURCE_BUFFER(data->editor_buffer));
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(data->editor), TRUE);
    gtk_container_add(GTK_CONTAINER(editor_scroll), data->editor);
    gtk_paned_add1(GTK_PANED(paned), editor_scroll);

    // Derecha: Output (solo lectura)
    GtkWidget *output_scroll = gtk_scrolled_window_new(NULL, NULL);
    data->output_buffer = gtk_text_buffer_new(NULL);
    data->output = gtk_text_view_new_with_buffer(data->output_buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(data->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(data->output), TRUE);
    gtk_container_add(GTK_CONTAINER(output_scroll), data->output);
    gtk_paned_add2(GTK_PANED(paned), output_scroll);

    gtk_widget_show_all(data->window);
}
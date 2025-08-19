#include "gui.h"  // Incluir el encabezado de la GUI
#include "../lang/lang.h"   /* <-- añadir al inicio del archivo (o al top) */

// Implementación de las funciones de callback ----------------------------------------------------------------------------------------------------
// Función para abrir archivos
static void on_open(GtkWidget *widget, AppWidgets *data) {
    // Crear un diálogo de selección de archivos
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Abrir Archivo", GTK_WINDOW(data->window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    "_Abrir", GTK_RESPONSE_ACCEPT, NULL);

    // Filtro para solo mostrar archivos .usl
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    // Mostrar el diálogo y esperar la respuesta
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
        
        // Actualizar título de ventana
        gchar *window_title = g_strdup_printf("JavaLang Interpreter - %s", g_path_get_basename(data->current_file));
        gtk_window_set_title(GTK_WINDOW(data->window), window_title);
        g_free(window_title);
    }
    gtk_widget_destroy(dialog);
}

// Función para guardar 
static void on_save(GtkWidget *widget, AppWidgets *data) {
    if (data->current_file == NULL) {
        on_save_as(widget, data);  // Si no hay archivo, usar Guardar Como
        return;
    }

    // Guardar contenido del editor en archivo actual
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(data->editor_buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(data->editor_buffer, &start, &end, FALSE);
    g_file_set_contents(data->current_file, text, -1, NULL);
    g_free(text);
    
    // Mostrar mensaje temporal en la salida
    gtk_text_buffer_set_text(data->output_buffer, "✓ Archivo guardado exitosamente.\n", -1);
}

// Función para guardar como
static void on_save_as(GtkWidget *widget, AppWidgets *data) {
    // Crear diálogo de selección de archivos
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Guardar Como", GTK_WINDOW(data->window),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    "_Guardar", GTK_RESPONSE_ACCEPT, NULL);
    // Filtro para solo mostrar archivos .usl
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.usl");
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    // Mostrar el diálogo y esperar la respuesta
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
        
        // Actualizar título de ventana
        gchar *window_title = g_strdup_printf("JavaLang Interpreter - %s", g_path_get_basename(data->current_file));
        gtk_window_set_title(GTK_WINDOW(data->window), window_title);
        g_free(window_title);
        
        // Mostrar mensaje temporal en la salida
        gtk_text_buffer_set_text(data->output_buffer, "✓ Archivo guardado como nuevo archivo exitosamente.\n", -1);
    }
    gtk_widget_destroy(dialog);
}

/* Función para ejecutar el código (reemplaza el placeholder) */
static void on_run(GtkWidget *widget, AppWidgets *data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(data->editor_buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(data->editor_buffer, &start, &end, FALSE);

    if (!text || strlen(text) == 0) {
        gtk_text_buffer_set_text(data->output_buffer, "⚠️  Editor vacío.\n", -1);
        g_free(text);
        return;
    }

    /* La calculadora espera una expresión terminada en ';' */
    /* Si el usuario no puso ';' al final, lo añadimos para conveniencia. */
    char *input = NULL;
    if (text[strlen(text)-1] != ';') {
        input = g_strdup_printf("%s;", text);
    } else {
        input = g_strdup(text);
    }

    char outbuf[1024];
    int rc = jl_run(input, outbuf, sizeof(outbuf));
    if (rc == 0) {
        /* resultado OK */
        gtk_text_buffer_set_text(data->output_buffer, outbuf, -1);
    } else {
        /* error léxico/sintáctico */
        gtk_text_buffer_set_text(data->output_buffer, outbuf, -1);
    }

    g_free(input);
    g_free(text);
}

// Función para generar el AST
static void on_generate_ast(GtkWidget *widget, AppWidgets *data) {
    //TODO Placeholder: Genera y muestra el AST
    gtk_text_buffer_set_text(data->output_buffer,
        "🌳 Generando Árbol de Sintaxis Abstracta (AST)...\n"
        "═══════════════════════════════════════════════════\n"
        "Analizando estructura del código...\n"
        "Construyendo nodos del árbol...\n"
        "Validando sintaxis...\n\n"
        "📋 Vista del AST:\n"
        "├─ Programa Principal\n"
        "│  ├─ Declaraciones\n"
        "│  ├─ Funciones\n"
        "│  └─ Bloque Principal\n"
        "│     ├─ Sentencias\n"
        "│     └─ Expresiones\n\n"
        "⚠️  Nota: Implementar generador de AST real\n", -1);
}

// Función para inicializar la GUI
void init_gui(GtkApplication *app, gpointer user_data) {
    AppWidgets *data = g_new(AppWidgets, 1);
    data->current_file = NULL;

    // Ventana principal
    data->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(data->window), "JavaLang Interpreter");
    gtk_window_set_default_size(GTK_WINDOW(data->window), 1400, 900);
    gtk_window_set_position(GTK_WINDOW(data->window), GTK_WIN_POS_CENTER);

    // Layout principaL
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(data->window), vbox);

    // ENCABEZADO CON TÍTULO PROMINENTE
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), header_box, FALSE, FALSE, 15);
    
    // Título principal con estilo
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), 
        "<span size='24000' weight='bold' foreground='#2E86AB'>☕ JAVALANG INTERPRETER</span>");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(header_box), title_label, FALSE, FALSE, 0);
    
    // Subtítulo descriptivo
    GtkWidget *subtitle_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(subtitle_label), 
        "<span size='11000' style='italic' foreground='#555555'>Entorno de desarrollo integrado para el lenguaje JavaLang</span>");
    gtk_widget_set_halign(subtitle_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(header_box), subtitle_label, FALSE, FALSE, 0);
    
    // Línea separadora
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(header_box), separator, FALSE, FALSE, 5);

    // Toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH); // Icono y texto
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 5);
    
    // Establecer espaciado y estilo de la toolbar
    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 8);

    // Botón Abrir Archivo
    GtkToolItem *open_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_LARGE_TOOLBAR), "Abrir Archivo");
    gtk_tool_item_set_tooltip_text(open_btn, "Abrir un archivo .usl existente");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_btn, -1);
    g_signal_connect(open_btn, "clicked", G_CALLBACK(on_open), data);

    // Separador
    GtkToolItem *sep1 = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep1, -1);

    // Botón Guardar
    GtkToolItem *save_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_LARGE_TOOLBAR), "Guardar");
    gtk_tool_item_set_tooltip_text(save_btn, "Guardar el archivo actual");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_btn, -1);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save), data);

    // Botón Guardar Como
    GtkToolItem *save_as_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_LARGE_TOOLBAR), "Guardar Como");
    gtk_tool_item_set_tooltip_text(save_as_btn, "Guardar con un nuevo nombre");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_as_btn, -1);
    g_signal_connect(save_as_btn, "clicked", G_CALLBACK(on_save_as), data);

    // Separador
    GtkToolItem *sep2 = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep2, -1);

    // Botón Ejecutar Código
    GtkToolItem *run_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_LARGE_TOOLBAR), "Ejecutar");
    gtk_tool_item_set_tooltip_text(run_btn, "Ejecutar el código JavaLang");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), run_btn, -1);
    g_signal_connect(run_btn, "clicked", G_CALLBACK(on_run), data);

    // Botón Generar AST
    GtkToolItem *ast_btn = gtk_tool_button_new(gtk_image_new_from_icon_name("view-list-tree", GTK_ICON_SIZE_LARGE_TOOLBAR), "Generar AST");
    gtk_tool_item_set_tooltip_text(ast_btn, "Mostrar el Árbol Sintáctico Abstracto");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ast_btn, -1);
    g_signal_connect(ast_btn, "clicked", G_CALLBACK(on_generate_ast), data);

    // Panel para dividir izquierda/derecha
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), paned, TRUE, TRUE, 5);
    gtk_paned_set_position(GTK_PANED(paned), 700);  // Posición inicial

    // EDITOR DE CÓDIGO
    GtkWidget *left_frame = gtk_frame_new("📝 Editor de Código");
    gtk_frame_set_label_align(GTK_FRAME(left_frame), 0.5, 0.5);
    
    GtkWidget *editor_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(editor_scroll), 
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    // Configurar highlighting de sintaxis
    GtkSourceLanguageManager *lang_manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage *lang = gtk_source_language_manager_get_language(lang_manager, "java");
    data->editor_buffer = GTK_TEXT_BUFFER(gtk_source_buffer_new(NULL));
    gtk_source_buffer_set_language(GTK_SOURCE_BUFFER(data->editor_buffer), lang);
    
    data->editor = gtk_source_view_new_with_buffer(GTK_SOURCE_BUFFER(data->editor_buffer));
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(data->editor), TRUE);
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(data->editor), TRUE);
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(data->editor), TRUE);
    gtk_source_view_set_auto_indent(GTK_SOURCE_VIEW(data->editor), TRUE);
    gtk_source_view_set_tab_width(GTK_SOURCE_VIEW(data->editor), 4);
    
    // Mensaje inicial en el editor
    gtk_text_buffer_set_text(data->editor_buffer, 
        "// Bienvenido al JavaLang Interpreter\n"
        "// Escribe tu código JavaLang aquí\n\n"
        "public class Main {\n"
        "    public static void main(String[] args) {\n"
        "        // Tu código aquí\n"
        "        System.out.println(\"¡Hola JavaLang!\");\n"
        "    }\n"
        "}\n", -1);
    
    gtk_container_add(GTK_CONTAINER(editor_scroll), data->editor);
    gtk_container_add(GTK_CONTAINER(left_frame), editor_scroll);
    gtk_paned_add1(GTK_PANED(paned), left_frame);

    // SALIDA Y RESULTADOS
    GtkWidget *right_frame = gtk_frame_new("📋 Salida y Resultados");
    gtk_frame_set_label_align(GTK_FRAME(right_frame), 0.5, 0.5);
    
    GtkWidget *output_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(output_scroll), 
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    data->output_buffer = gtk_text_buffer_new(NULL);
    data->output = gtk_text_view_new_with_buffer(data->output_buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(data->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(data->output), TRUE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(data->output), FALSE);
    
    // Mensaje de bienvenida en la salida
    gtk_text_buffer_set_text(data->output_buffer,
        "  JavaLang Interpreter v1.0\n"
        "═════════════════════════════\n\n"
        "¡Bienvenido al intérprete JavaLang!\n\n"

        "• Usa 'Abrir Archivo' para cargar un archivo .usl\n"
        "• Escribe tu código en el editor\n"
        "• Presiona 'Ejecutar' para procesar el programa\n"
        "• Usa 'Generar AST' para ver el árbol sintáctico abstracto\n\n", -1);
    
    gtk_container_add(GTK_CONTAINER(output_scroll), data->output);
    gtk_container_add(GTK_CONTAINER(right_frame), output_scroll);
    gtk_paned_add2(GTK_PANED(paned), right_frame);

    // BARRA DE ESTADO
    GtkWidget *status_bar = gtk_statusbar_new();
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "Listo - JavaLang Interpreter iniciado");
    gtk_box_pack_start(GTK_BOX(vbox), status_bar, FALSE, FALSE, 0);

    // Mostrar toda la interfazt
    gtk_widget_show_all(data->window);
}
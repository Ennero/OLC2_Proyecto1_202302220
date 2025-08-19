#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

// Estructura para almacenar widgets principales 
typedef struct {
    GtkWidget *window;
    GtkWidget *editor;  // Entrada
    GtkWidget *output;  // Salida
    GtkTextBuffer *editor_buffer;
    GtkTextBuffer *output_buffer;
    gchar *current_file;  // Archivo actual
} AppWidgets;

// Definiciones de las funciones de callback
static void on_open(GtkWidget *widget, AppWidgets *data);
static void on_save(GtkWidget *widget, AppWidgets *data);
static void on_save_as(GtkWidget *widget, AppWidgets *data);
static void on_run(GtkWidget *widget, AppWidgets *data);
static void on_generate_ast(GtkWidget *widget, AppWidgets *data);

// Función para inicializar la GUI
void init_gui(GtkApplication *app, gpointer user_data);

#endif
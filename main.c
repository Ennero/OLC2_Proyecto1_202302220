#include <gtk/gtk.h>

void init_gui(GtkApplication *app, gpointer user_data);  // Declaración de la función en gui.c

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.example.javalang", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(init_gui), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
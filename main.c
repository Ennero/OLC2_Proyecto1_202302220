#include <gtk/gtk.h>
#include "gui/gui.h"  // Nueva inclusión con la ruta relativa

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.example.javalang", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(init_gui), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
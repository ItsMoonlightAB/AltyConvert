#ifndef CONVERTER_H
#define CONVERTER_H
#include <gtk/gtk.h>

void process_conversion(const char *input_path, const char *target_ext, GtkWidget *label_status, GtkWidget *btn_reveal, GtkWidget *progress_bar, GtkWidget *window);
#endif
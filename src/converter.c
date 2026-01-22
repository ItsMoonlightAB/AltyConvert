#include <windows.h>
#include <shlobj.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "converter.h"
#include "utils.h"
#include "conv_video.h"
#include "conv_audio.h"
#include "conv_image.h"

typedef struct {
    char *input, *ext, *output_path;
    GtkWidget *label, *btn_reveal, *progress_bar;
    gboolean success;
} ThreadData;

static void on_reveal_clicked(GtkWidget *btn, gpointer user_data) {
    char *path = (char *)user_data;
    if (!path) return;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    wchar_t *w_path = (wchar_t *)g_utf8_to_utf16(path, -1, NULL, NULL, NULL);
    if (w_path) {
        ITEMIDLIST *pidl = ILCreateFromPathW(w_path);
        if (pidl) { SHOpenFolderAndSelectItems(pidl, 0, NULL, 0); ILFree(pidl); }
        g_free(w_path);
    }
    CoUninitialize();
}

static gboolean on_conversion_finished(gpointer user_data) {
    ThreadData *data = (ThreadData *)user_data;
    gtk_widget_set_visible(data->progress_bar, FALSE);

    if (data->success) {
        gtk_label_set_text(GTK_LABEL(data->label), "Done !");
        gtk_widget_set_visible(data->btn_reveal, TRUE);
        
        g_signal_handlers_disconnect_by_func(data->btn_reveal, G_CALLBACK(on_reveal_clicked), NULL);
        g_signal_connect_data(data->btn_reveal, "clicked", G_CALLBACK(on_reveal_clicked), g_strdup(data->output_path), (GClosureNotify)g_free, 0);
    } else {
        gtk_label_set_text(GTK_LABEL(data->label), "Failed");
    }

    g_free(data->input); 
    g_free(data->ext); 
    g_free(data->output_path);
    g_free(data);
    return FALSE;
}

static gpointer conversion_worker(gpointer user_data) {
    ThreadData *data = (ThreadData *)user_data;
    data->output_path = get_output_filename(data->input, data->ext);
    
    if (data->output_path) {
        gboolean result = FALSE;
        if (g_strv_contains((const char*[]){"mp4", "mkv", "avi", "mov", "ogg", "wmv", "webm", NULL}, data->ext)) {
            result = convert_video(data->input, data->output_path);
        } else if (g_strv_contains((const char*[]){"mp3", "wav", "ogg", NULL}, data->ext)) {
            result = convert_audio(data->input, data->output_path);
        } else {
            result = convert_image(data->input, data->output_path);
        }
        
        if (result && g_file_test(data->output_path, G_FILE_TEST_EXISTS)) {
            data->success = TRUE;
        }
    }
    g_idle_add(on_conversion_finished, data);
    return NULL;
}

void process_conversion(const char *in, const char *ext, GtkWidget *lbl, GtkWidget *btn, GtkWidget *pb, GtkWidget *win) {
    ThreadData *data = g_new0(ThreadData, 1);
    data->input = g_strdup(in); 
    data->ext = g_strdup(ext);
    data->label = lbl; 
    data->btn_reveal = btn; 
    data->progress_bar = pb;

    gtk_widget_set_visible(btn, FALSE);
    gtk_widget_set_visible(pb, TRUE);
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(pb));
    gtk_label_set_text(GTK_LABEL(lbl), "Converting...");

    g_thread_new("conv_thread", conversion_worker, data);
}
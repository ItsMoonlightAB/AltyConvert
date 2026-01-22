#include <gtk/gtk.h>
#include "converter.h"
#include "utils.h"

typedef struct {
    GtkWidget *window, *label_path, *dropdown_format, *progress_bar, *btn_reveal, *stack, *surface;
    char *selected_input_path;
} AppWidgets;

typedef enum { STATUS_INFO, STATUS_ERROR, STATUS_SUCCESS } StatusType;
typedef enum { TYPE_IMAGE, TYPE_VIDEO, TYPE_AUDIO, TYPE_UNKNOWN } FileType;

const char *material_css =
    "window {"
        "background-color: #212121;"
        "color: #ffffff;"
        "font-family: 'Segoe UI Variable',"
        "sans-serif; font-size: 15px;"
    "}"
    
    ".main-container {"
        "padding: 20px 25px;"
    "}"

    ".surface {"
        "background-color: #292929;"
        "border-radius: 28px;"
        "padding: 25px;"
        "border-width: 2px;"
        "border-style: solid;"
        "border-color: transparent;"
        "transition: all 200ms ease;"
    "}"

    ".surface.drag-hover {"
        "background-color: #333333 !important;"
        "border-style: dashed !important;"
        "border-color: #006AF5 !important;"
        "transform: scale(1.05);"
        "box-shadow: 0 0 30px rgba(255, 255, 255, 0.25);"
    "}"

    ".placeholder-text {"
        "font-size: 24px;"
        "font-weight: 800;"
        "color: #555555;"
    "}"

    "#status-label {"
        "font-weight: 700;"
        "font-size: 16px; color: #ffffff;"
    "}" 

    "dropdown > button {"
        "border-radius: 15px;"
        "border: none;"
        "background: #333;"
        "color: white;"
        "padding: 12px 20px;"
        "transition: 200ms;"
    "}"

    "dropdown > button:hover {"
        "background-color: #444;"
    "}"

    "popover {"
        "background-color: #292929;"
        "color: white;"
        "border: 1px solid #444;"
    "}"

    "popover contents {"
        "background-color: #292929;"
    "}"

    "popover listview {"
        "background-color: #292929;"
        "color: white;"
    "}"

    "popover listview > row:hover, popover listview > row:selected {"
        "background-color: #006AF5;"
        "color: white;"
    "}"
    
    "button.suggested-action {"
        "background-color: #006AF5;"
        "color: white !important;"
        "border-radius: 16px; padding: 12px; font-weight: 800; border: none;"
        "transition: all 100ms ease;"
    "}"
    
    "button.suggested-action:hover {"
        "background-image: none !important;"
        "background-color: #444444 !important;"
        "color: #ffffff !important;"
        "box-shadow: none;"
    "}"
    
    "button.suggested-action:active {"
        "transform: scale(0.96);"
        "background-color: #222222 !important;"
    "}"
    
    "button.secondary {"
        "border-radius: 16px; background: #333; color: white; border: none;"
        "padding: 10px 20px; font-size: 14px;"
        "transition: all 100ms ease;"
    "}"
    
    "button.secondary:hover {"
        "background-color: #444;"
        "background-image: none;"
    "}"
    
    "button.secondary:active {"
        "transform: scale(0.96);"
        "background-color: #222222 !important;"
    "}"

    "progressbar progress {"
        "background-color: #006AF5;"
    "}";

const char* img_fmts[] = {"jpg", "jpeg", "png", "gif", "tiff", "ico", "webp", NULL};
const char* vid_fmts[] = {"mp4", "mkv", "avi", "mov", "ogg", "wmv", "webm", NULL};
const char* aud_fmts[] = {"mp3", "wav", "ogg", NULL};

static void update_status(AppWidgets *widgets, const char *text, StatusType type) {
    gtk_label_set_text(GTK_LABEL(widgets->label_path), text);
    
    gtk_widget_remove_css_class(widgets->label_path, "error-text");
    gtk_widget_remove_css_class(widgets->label_path, "success-text");
}

static FileType get_file_type(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return TYPE_UNKNOWN;
    char *ext = g_ascii_strdown(dot, -1);
    FileType type = TYPE_UNKNOWN;

    if (g_strv_contains((const char*[]){".jpg", ".jpeg", ".png", ".gif", ".tiff", ".ico", ".webp", NULL}, ext)) type = TYPE_IMAGE;
    else if (g_strv_contains((const char*[]){".mp4", ".mkv", ".avi", ".mov", ".wmv", ".webm", ".ogg", NULL}, ext)) type = TYPE_VIDEO;
    else if (g_strv_contains((const char*[]){".mp3", ".wav", ".ogg", NULL}, ext)) type = TYPE_AUDIO;

    g_free(ext);
    return type;
}

static void update_ui_with_file(AppWidgets *widgets, GFile *file) {
    char *path = g_file_get_path(file);
    FileType type = get_file_type(path);

    if (type == TYPE_UNKNOWN) {
        update_status(widgets, "Format not supported", STATUS_ERROR);
        g_free(path); return;
    }

    g_free(widgets->selected_input_path);
    widgets->selected_input_path = path;
    
    GtkStringList *new_model;
    if (type == TYPE_IMAGE) new_model = gtk_string_list_new(img_fmts);
    else if (type == TYPE_VIDEO) new_model = gtk_string_list_new(vid_fmts);
    else new_model = gtk_string_list_new(aud_fmts);
    
    gtk_drop_down_set_model(GTK_DROP_DOWN(widgets->dropdown_format), G_LIST_MODEL(new_model));

    char *name = g_file_get_basename(file);
    update_status(widgets, name, STATUS_INFO);
    g_free(name);
    gtk_stack_set_visible_child_name(GTK_STACK(widgets->stack), "file-selected");
}

static void on_drag_enter(GtkDropTarget *t, double x, double y, gpointer d) { gtk_widget_add_css_class(((AppWidgets*)d)->surface, "drag-hover"); }
static void on_drag_leave(GtkDropTarget *t, gpointer d) { gtk_widget_remove_css_class(((AppWidgets*)d)->surface, "drag-hover"); }
static gboolean on_drop(GtkDropTarget *t, const GValue *v, double x, double y, gpointer d) {
    gtk_widget_remove_css_class(((AppWidgets*)d)->surface, "drag-hover");
    if (G_VALUE_HOLDS(v, G_TYPE_FILE)) { update_ui_with_file((AppWidgets*)d, g_value_get_object(v)); return TRUE; }
    return FALSE;
}

static void on_open_response(GObject *src, GAsyncResult *res, gpointer data) {
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(src), res, NULL);
    if (file) { update_ui_with_file((AppWidgets*)data, file); g_object_unref(file); }
}

static void on_browse_clicked(GtkWidget *b, gpointer data) {
    gtk_file_dialog_open(gtk_file_dialog_new(), GTK_WINDOW(((AppWidgets*)data)->window), NULL, on_open_response, data);
}

static void on_convert_clicked(GtkWidget *b, gpointer data) {
    AppWidgets *w = (AppWidgets*)data;
    if (!w->selected_input_path) { 
        update_status(w, "Select a file", STATUS_ERROR); 
        gtk_stack_set_visible_child_name(GTK_STACK(w->stack), "file-selected");
        return; 
    }
    const char *ext = gtk_string_list_get_string(GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(w->dropdown_format))), gtk_drop_down_get_selected(GTK_DROP_DOWN(w->dropdown_format)));
    process_conversion(w->selected_input_path, ext, w->label_path, w->btn_reveal, w->progress_bar, w->window);
}

static void on_activate(GtkApplication *app, gpointer data) {
    GtkCssProvider *p = gtk_css_provider_new();
    gtk_css_provider_load_from_string(p, material_css);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(p), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    AppWidgets *w = g_new0(AppWidgets, 1);
    w->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(w->window), "AltyConvert");
    gtk_window_set_default_size(GTK_WINDOW(w->window), 500, -1);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_add_css_class(main_box, "main-container");
    gtk_window_set_child(GTK_WINDOW(w->window), main_box);

    w->surface = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(w->surface, "surface");
    gtk_box_append(GTK_BOX(main_box), w->surface);

    w->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(w->stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    
    GtkWidget *empty_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_valign(empty_box, GTK_ALIGN_CENTER);
    GtkWidget *lbl_dd = gtk_label_new("Drag & Drop");
    gtk_widget_add_css_class(lbl_dd, "placeholder-text");
    gtk_box_append(GTK_BOX(empty_box), lbl_dd);
    gtk_stack_add_named(GTK_STACK(w->stack), empty_box, "empty");

    w->label_path = gtk_label_new("");
    gtk_widget_set_name(w->label_path, "status-label");
    gtk_label_set_ellipsize(GTK_LABEL(w->label_path), PANGO_ELLIPSIZE_MIDDLE);
    gtk_stack_add_named(GTK_STACK(w->stack), w->label_path, "file-selected");
    gtk_box_append(GTK_BOX(w->surface), w->stack);

    GtkWidget *btn_br = gtk_button_new_with_label("Browse files");
    gtk_widget_add_css_class(btn_br, "secondary");
    gtk_widget_set_margin_top(btn_br, 15);
    g_signal_connect(btn_br, "clicked", G_CALLBACK(on_browse_clicked), w);
    gtk_box_append(GTK_BOX(w->surface), btn_br);

    GtkWidget *bot = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_append(GTK_BOX(bot), gtk_label_new("Convert to"));
    
    w->dropdown_format = gtk_drop_down_new_from_strings((const char*[]){"Select a file", NULL});
    gtk_box_append(GTK_BOX(bot), w->dropdown_format);

    w->progress_bar = gtk_progress_bar_new();
    gtk_widget_set_visible(w->progress_bar, FALSE);
    gtk_box_append(GTK_BOX(bot), w->progress_bar);

    GtkWidget *btn_cv = gtk_button_new_with_label("Start conversion");
    gtk_widget_add_css_class(btn_cv, "suggested-action");
    g_signal_connect(btn_cv, "clicked", G_CALLBACK(on_convert_clicked), w);
    gtk_box_append(GTK_BOX(bot), btn_cv);

    w->btn_reveal = gtk_button_new_with_label("Open folder");
    gtk_widget_add_css_class(w->btn_reveal, "secondary");
    gtk_widget_set_visible(w->btn_reveal, FALSE);
    gtk_box_append(GTK_BOX(bot), w->btn_reveal);
    gtk_box_append(GTK_BOX(main_box), bot);

    GtkDropTarget *tgt = gtk_drop_target_new(G_TYPE_FILE, GDK_ACTION_COPY);
    g_signal_connect(tgt, "enter", G_CALLBACK(on_drag_enter), w);
    g_signal_connect(tgt, "leave", G_CALLBACK(on_drag_leave), w);
    g_signal_connect(tgt, "drop", G_CALLBACK(on_drop), w);
    gtk_widget_add_controller(w->window, GTK_EVENT_CONTROLLER(tgt));
    gtk_window_present(GTK_WINDOW(w->window));
}

int main(int argc, char **argv) {
    g_setenv("GTK_CSD", "0", TRUE);
    GtkApplication *app = gtk_application_new("com.altyconvert.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
#include <glib.h>
#include <string.h>
#include "conv_image.h"

gboolean convert_image(const char *input_path, const char *output_path) {
    char *ext = g_ascii_strdown(strrchr(output_path, '.') + 1, -1);
    gboolean is_ico = g_str_equal(ext, "ico");

    char *argv[15];
    int i = 0;
    argv[i++] = "ffmpeg";
    argv[i++] = "-y";
    argv[i++] = "-i";
    argv[i++] = (char *)input_path;
    
    if (is_ico) {
        argv[i++] = "-vf";
        argv[i++] = "scale=256:256:flags=lanczos,format=bgra";
    }

    argv[i++] = "-update";
    argv[i++] = "1";
    argv[i++] = (char *)output_path;
    argv[i++] = NULL;

    int exit_status;
    gboolean spawned = g_spawn_sync(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, &exit_status, NULL);

    g_free(ext);
    return (spawned && exit_status == 0);
}
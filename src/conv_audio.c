#include <glib.h>
#include <string.h>
#include "conv_audio.h"

gboolean convert_audio(const char *input_path, const char *output_path) {
    char *ext = g_ascii_strdown(strrchr(output_path, '.') + 1, -1);
    
    char *argv[15];
    int i = 0;
    argv[i++] = "ffmpeg";
    argv[i++] = "-y";
    argv[i++] = "-i";
    argv[i++] = (char *)input_path;

    if (g_str_equal(ext, "mp3")) {
        argv[i++] = "-c:a"; argv[i++] = "libmp3lame";
        argv[i++] = "-q:a"; argv[i++] = "2";
    } else if (g_str_equal(ext, "ogg")) {
        argv[i++] = "-c:a"; argv[i++] = "libvorbis";
        argv[i++] = "-q:a"; argv[i++] = "4";
    }

    argv[i++] = (char *)output_path;
    argv[i++] = NULL;

    int exit_status;
    gboolean spawned = g_spawn_sync(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, &exit_status, NULL);

    g_free(ext);
    return (spawned && exit_status == 0);
}
#include <glib.h>
#include <string.h>
#include "conv_video.h"

gboolean convert_video(const char *input_path, const char *output_path) {
    char *ext = g_ascii_strdown(strrchr(output_path, '.') + 1, -1);
    
    char *argv[20];
    int i = 0;
    argv[i++] = "ffmpeg";
    argv[i++] = "-y";
    argv[i++] = "-i";
    argv[i++] = (char *)input_path;

    if (g_str_equal(ext, "webm")) {
        argv[i++] = "-c:v"; argv[i++] = "libvpx";
        argv[i++] = "-b:v"; argv[i++] = "1M";
        argv[i++] = "-c:a"; argv[i++] = "libvorbis";
    } else if (g_str_equal(ext, "wmv")) {
        argv[i++] = "-c:v"; argv[i++] = "wmv2";
        argv[i++] = "-b:v"; argv[i++] = "2M";
        argv[i++] = "-c:a"; argv[i++] = "wmav2";
    } else {
        argv[i++] = "-c:v"; argv[i++] = "libx264";
        argv[i++] = "-preset"; argv[i++] = "fast";
        argv[i++] = "-crf"; argv[i++] = "23";
        argv[i++] = "-pix_fmt"; argv[i++] = "yuv420p";
        argv[i++] = "-c:a"; argv[i++] = "aac";
    }

    argv[i++] = (char *)output_path;
    argv[i++] = NULL;

    int exit_status;
    gboolean spawned = g_spawn_sync(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, &exit_status, NULL);

    g_free(ext);
    return (spawned && exit_status == 0);
}
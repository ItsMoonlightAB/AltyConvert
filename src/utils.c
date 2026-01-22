#include <string.h>
#include <glib.h>
#include "utils.h"

char* get_output_filename(const char* input_path, const char* new_ext) {
    char *base_path = g_strdup(input_path);
    char *last_dot = strrchr(base_path, '.');
    if (last_dot) *last_dot = '\0';

    char *output = g_strdup_printf("%s.%s", base_path, new_ext);

    int counter = 1;
    while (g_file_test(output, G_FILE_TEST_EXISTS)) {
        g_free(output);
        output = g_strdup_printf("%s (%d).%s", base_path, counter++, new_ext);
    }

    g_free(base_path);
    return output;
}
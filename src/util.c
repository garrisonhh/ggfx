#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static FILE *gg_resource(const char *resource_path) {
    const char *this_file = __FILE__;
    const char *trav = this_file;
    size_t this_len = 0;
    bool found_slash = false;

    while (*trav++)
        ++this_len;

    while (true) {
        --trav;

        if (*trav == '/' || *trav == '\\') {
            if (found_slash)
                break;
            else
                found_slash = true;
        }

        --this_len;
    }

    // this_len now represents length of string up to home folder for ggfx
    // now copy home folder to path_buf, add "res/", and then resource path
    char path_buf[500];
    FILE *fp = NULL;

    gg_strcpy(path_buf, this_file);
    gg_strcpy(path_buf + this_len, "/res/");
    gg_strcpy(path_buf + this_len + 5, resource_path);

    if (!(fp = fopen(path_buf, "r")))
        GG_ERROR("could not open \"%s\".\n", path_buf);

    return fp;
}

// finds exact length of file and returns string malloc'd with exact num bytes
char *gg_load_file(const char *filename, size_t *out_len) {
    FILE *fp = gg_resource(filename);

    fseek(fp, 0, SEEK_END);

    size_t len = ftell(fp);
    char *str = malloc(sizeof(*str) * len + 1);

    rewind(fp);
    fread(str, sizeof(*str), len, fp);
    fclose(fp);

    str[len] = '\0';

    if (out_len)
        *out_len = len;

    return str;
}

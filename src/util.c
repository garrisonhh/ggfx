#include <stdio.h>
#include <stdlib.h>

// finds exact length of file and returns string malloc'd with exact num bytes
char *gg_load_file(const char *filename, size_t *out_len) {
    FILE *fp = fopen(filename, "r");

    if (!fp)
        GG_ERROR("could not open \"%s\".\n", filename);

    fseek(fp, 0, SEEK_END);

    size_t len = ftell(fp);
    char *str = malloc(sizeof(*str) * len);

    fread(str, sizeof(*str), len, fp);
    fclose(fp);

    if (out_len)
        *out_len = len;

    return str;
}

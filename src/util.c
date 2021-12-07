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

static uint32_t gg_hash(char *str) {
    const uint32_t FNV_PRIME = 0x01000193a;
    const uint32_t FNV_BASIS = 0x0811c9dc5;

    uint32_t hash = FNV_BASIS;

    while (*str)
        hash = (hash ^ *str++) * FNV_PRIME;

    return hash;
}

#if 0
void gg_map_make(gg_map_t *map) {
#ifndef GG_MAP_INIT_CAP
#define GG_MAP_INIT_CAP 8
#endif
    map->cap = GG_MAP_INIT_CAP;
    map->size = 0;
    map->nodes = calloc(map->cap, sizeof(*map->nodes));
}

void gg_map_kill(gg_map_t *map) {
    free(map->nodes);
}

static void gg_map_put_hash(gg_map_t *map, uint32_t hash, void *data) {
    size_t idx = hash % map->cap;

    while (map->nodes[idx].data && hash != map->nodes[idx].hash)
        idx = (idx + 1) % map->cap;

    map->nodes[idx].hash = hash;
    map->nodes[idx].data = data;
}

static void gg_map_grow(gg_map_t *map) {
    size_t old_cap = map->cap;
    gg_mapnode_t *old_nodes = map->nodes;

    map->cap *= 2;
    map->nodes = calloc(map->cap, sizeof(*old_nodes));

    for (size_t i = 0; i < old_cap; ++i)
        if (old_nodes[i].data)
            gg_map_put_hash(map, old_nodes[i].hash, old_nodes[i].data);

    free(old_nodes);
}

void gg_map_put(gg_map_t *map, char *key, void *data) {
    if (map->size + 1 >= map->cap / 2)
        gg_map_grow(map);

    gg_map_put_hash(map, gg_hash(key), data);
}

void *gg_map_get(gg_map_t *map, char *key) {
    uint32_t hash = gg_hash(key);
    size_t idx = hash % map->cap;

    while (map->nodes[idx].data && hash != map->nodes[idx].hash)
        idx = (idx + 1) % map->cap;

    return map->nodes[idx].data;
}
#endif

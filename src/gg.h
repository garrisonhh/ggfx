#ifndef GG_H
#define GG_H

#include <SDL2/SDL.h>

typedef struct gg_config {
    const char *window_name;

    int window_width, window_height;

    unsigned maximize: 1;
    unsigned disable_vsync: 1;
    unsigned enable_depth: 1;
} gg_config_t;

void gg_init(gg_config_t);
#define gg_init_default() gg_init((gg_config_t){0})
void gg_quit(void);

#endif

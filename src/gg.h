#ifndef GG_H
#define GG_H

/*
 * gg.* provides initialization and configuration functions, and manages global
 * variables like the SDL_Window and SDL_GLContext.
 */

#include <stdbool.h>
#include <SDL2/SDL.h>

extern SDL_Window *gg_window;
extern SDL_GLContext *gg_gl_ctx;

// init config uses ZII heavily, so options all have defaults if set to zero
typedef struct gg_config {
    const char *window_name;
    int window_width, window_height;

    unsigned maximize: 1;
    unsigned disable_vsync: 1;
    unsigned enable_depth: 1;
} gg_config_t;

void gg_init(gg_config_t);
void gg_quit(void);

static inline void gg_flip(void) { SDL_GL_SwapWindow(gg_window); }

// use instead of SDL_PollEvent or ggfx won't work correctly.
bool gg_poll_event(SDL_Event *);

#endif

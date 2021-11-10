#ifndef GG_H
#define GG_H

/*
 * gg.* provides initialization and configuration functions, and manages global
 * state.
 */

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "../gglm.h"

extern GLuint gg_bound_fbo;
extern v2 gg_bound_fbo_size;
extern v2 gg_window_size;
extern SDL_Window *gg_window;
extern SDL_GLContext *gg_gl_ctx;

// init config uses ZII heavily, so options all have defaults if set to zero
typedef struct gg_config {
    const char *window_name;
    v2 window_size;

    unsigned maximize: 1;
    unsigned disable_vsync: 1;
    unsigned enable_depth: 1;
} gg_config_t;

void gg_init(gg_config_t);
void gg_quit(void);

// framebuffer functionality is done through texture.*, this is for integration
// with the global state
void gg__set_bound_fbo(GLuint fbo, v2 size);
void gg__reset_bound_fbo(void);

void gg_clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

static inline void gg_flip(void) { SDL_GL_SwapWindow(gg_window); }

// use instead of SDL_PollEvent or ggfx won't work correctly.
bool gg_poll_event(SDL_Event *);

#endif

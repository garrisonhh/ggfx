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

#include "data.h"

extern SDL_Window *gg_window;
extern SDL_GLContext *gg_gl_ctx;

extern v2 gg_window_size;
extern GLbitfield gg_buffer_bits;

extern GLuint gg_bound_fbo;
extern v2 gg_resolution;

extern gg_pages_t gg_pool;

// init config uses ZII heavily, all options have defaults if set to zero
typedef struct gg_config {
    const char *title;
    v2 window_size; // set initial window size
    v2 resolution; // if set, window size can change but not resolution

    unsigned maximize: 1;
    unsigned disable_vsync: 1;
    unsigned enable_depth: 1;
} gg_config_t;

// location is encoded in number used
typedef enum gg_coords {
    GG_TOP_LEFT         = 0x00,
    GG_TOP_MIDDLE       = 0x01,
    GG_TOP_RIGHT        = 0x02,
    GG_MIDDLE_RIGHT     = 0x12,
    GG_BOTTOM_RIGHT     = 0x22,
    GG_BOTTOM_MIDDLE    = 0x21,
    GG_BOTTOM_LEFT      = 0x20,
    GG_MIDDLE_LEFT      = 0x10,
    GG_CENTER           = 0x11
} gg_coord_e;

void gg_init(gg_config_t);
void gg_quit(void);

// useful for placing things on screen relative to window locations
v2 gg_coord(gg_coord_e);

// framebuffer functionality is done through texture.*, this is for integration
// with the global state
// (double underscore == don't touch externally!)
void gg__set_bound_fbo(GLuint fbo, v2 size);
void gg__reset_bound_fbo(void);

void gg_clear(uint8_t, uint8_t, uint8_t, uint8_t);
void gg_flip(void);

// use instead of SDL_PollEvent or ggfx won't work correctly
bool gg_poll_event(SDL_Event *);
v2 gg_mouse_pos(void);

#endif

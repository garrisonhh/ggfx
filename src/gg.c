#include <stdlib.h>
#include <stdint.h>

#include "../ggfx.h"

GLuint gg_bound_fbo = 0;
v2 gg_bound_fbo_size;
v2 gg_window_size;

SDL_Window *gg_window = NULL;
SDL_GLContext *gg_gl_ctx = NULL;

static GLbitfield gg_clear_bits = GL_COLOR_BUFFER_BIT;

static void on_resize(void);

// initialize and configure an SDL2 window and OpenGL context
void gg_init(gg_config_t cfg) {
    /*
     * SDL2 config
     */
    uint32_t subsystems = SDL_WasInit(SDL_INIT_EVERYTHING);

    GG_ASSERT(
        subsystems & SDL_INIT_VIDEO,
        "SDL must be initialized before ggfx.\n"
    );

    // create window
    GG_SDL(gg_window = SDL_CreateWindow(
        cfg.window_name ? cfg.window_name : "ggfx app",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        cfg.window_size.x ? (int)cfg.window_size.x : 640,
        cfg.window_size.y ? (int)cfg.window_size.y : 480,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    ));

    if (cfg.maximize)
        SDL_MaximizeWindow(gg_window);

    // create gl ctx
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
    );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    GG_SDL(gg_gl_ctx = SDL_GL_CreateContext(gg_window));

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
        GG_ERROR("couldn't load OpenGL.\n");

    /*
     * OpenGL config
     */
    // opengl features
    if (!cfg.disable_vsync)
        if (SDL_GL_SetSwapInterval(-1) && SDL_GL_SetSwapInterval(1))
            GG_ERROR("unable to enable vsync.\n");

    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    if (cfg.enable_depth) {
        gg_clear_bits |= GL_DEPTH_BUFFER_BIT;

        GL(glEnable(GL_DEPTH_TEST));
        GL(glDepthFunc(GL_LESS));
    }

    // window sizing/resolution
    on_resize();

    ;
}

void gg_quit(void) {
    SDL_GL_DeleteContext(gg_gl_ctx);
    SDL_DestroyWindow(gg_window);
}

void gg__set_bound_fbo(GLuint fbo, v2 size) {
    gg_bound_fbo = fbo;
    gg_bound_fbo_size = size;
}

void gg__reset_bound_fbo(void) {
    gg_bound_fbo = 0;
    gg_bound_fbo_size = gg_window_size;
}

void gg_clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#define X(v) ((float)(v) / 255.0)
    GL(glClearColor(X(r), X(g), X(b), X(a)));
#undef X
    GL(glClear(gg_clear_bits));
}

static void on_resize(void) {
    int width, height;

    SDL_GetWindowSize(gg_window, &width, &height);

    GL(glViewport(0, 0, width, height));
    gg_window_size = v2_(width, height);

    if (!gg_bound_fbo)
        gg_bound_fbo_size = gg_window_size;
}

// this function dispatches internal event callbacks
bool gg_poll_event(SDL_Event *event) {
    if (!SDL_PollEvent(event))
        return false;

    switch (event->type) {
    case SDL_WINDOWEVENT:
        if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            on_resize();

        break;
    }

    return true;
}

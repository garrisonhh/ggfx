#include <stdlib.h>
#include <stdint.h>

#include "gg.h"
#include "events.h"
#include "util.h"

SDL_Window *gg_window = NULL;
SDL_GLContext *gg_gl_ctx = NULL;

static void gg_on_resize(void);
void gg_on_windowevent(SDL_Event *);

// initialize and configure an SDL2 window and OpenGL context
void gg_init(gg_config_t cfg) {
    // config ZII checks
    if (cfg.window_name == (const char *)0)
        cfg.window_name = "ggfx app";

    uint32_t subsystems = SDL_WasInit(SDL_INIT_EVERYTHING);

    GG_ASSERT(
        (subsystems & SDL_INIT_VIDEO) && (subsystems & SDL_INIT_EVENTS),
        "SDL must be initialized before ggfx.\n"
    );

    // create window
    GG_SDL_CHECK(gg_window = SDL_CreateWindow(
        cfg.window_name,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        cfg.window_width, cfg.window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    ));

    // create gl ctx
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
    );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    GG_SDL_CHECK(gg_gl_ctx = SDL_GL_CreateContext(gg_window));

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
        GG_ERROR("couldn't load OpenGL.\n");

    // OpenGL config
    if (!cfg.disable_vsync)
        if (SDL_GL_SetSwapInterval(-1) && SDL_GL_SetSwapInterval(1))
            GG_ERROR("unable to enable vsync.\n");

    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    if (cfg.enable_depth) {
        GL(glEnable(GL_DEPTH_TEST));
        GL(glDepthFunc(GL_LESS));
    }

    // initial window sizing
    SDL_SetWindowSize(
        gg_window,
        cfg.window_width ? cfg.window_width : 640,
        cfg.window_height ? cfg.window_height : 480
    );

    gg_on_resize();

    // default events
    gg_events_hook(SDL_WINDOWEVENT, gg_on_windowevent);
}

void gg_quit(void) {
    SDL_GL_DeleteContext(gg_gl_ctx);
    SDL_DestroyWindow(gg_window);
}

static void gg_on_resize(void) {
    int width, height;

    SDL_GetWindowSize(gg_window, &width, &height);

    GL(glViewport(0, 0, width, height));
}

void gg_on_windowevent(SDL_Event *event) {
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        gg_on_resize();
        break;
    default:
        break;
    }
}

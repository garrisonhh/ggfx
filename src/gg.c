#include <stdlib.h>
#include <stdint.h>

#include "gg.h"
#include "util.h"

SDL_Window *gg_window = NULL;
SDL_GLContext *gg_gl_ctx = NULL;

void gg_init(gg_config_t cfg) {
    // config ZII checks
    if (cfg.window_name == (const char *)0)
        cfg.window_name = "ghh_gl project";

    if (!cfg.window_width)
        cfg.window_width = 640;

    if (!cfg.window_height)
        cfg.window_height = 480;

    GG_ASSERT(
        SDL_WasInit(SDL_INIT_EVERYTHING) & SDL_INIT_VIDEO,
        "SDL must be initialized before ghh_gl.\n"
    );

    // create window
    GG_SDL_CHECK(gg_window = SDL_CreateWindow(
        cfg.window_name,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        cfg.window_width, cfg.window_height,
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
}

void gg_quit(void) {
    SDL_GL_DeleteContext(gg_gl_ctx);

    SDL_DestroyWindow(gg_window);
}

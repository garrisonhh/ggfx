#include <stdlib.h>
#include <stdint.h>

#include "gg.h"
#include "util.h"

SDL_Window *gg_window = NULL;
SDL_GLContext *gg_gl_ctx = NULL;

// initialize and configure an SDL2 window and OpenGL context
void gg_init(gg_config_t cfg) {
    // config ZII checks
    if (cfg.window_name == (const char *)0)
        cfg.window_name = "ghh_gl app";

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
}

void gg_quit(void) {
    SDL_GL_DeleteContext(gg_gl_ctx);

    SDL_DestroyWindow(gg_window);
}

void gg_on_resize(void) {
    int draw_width, draw_height;

    SDL_GL_GetDrawableSize(gg_window, &draw_width, &draw_height);

    printf("draw size: %d %d\n", draw_width, draw_height);

    GL(glViewport(0, 0, draw_width, draw_height));
}

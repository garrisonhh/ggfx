#include <stdlib.h>
#include <stdbool.h>

#include "../ggfx.h"

GLuint gg_bound_fbo = 0;
v2 gg_window_size, gg_resolution;

// static resolution vars
static bool gg_static_resolution = false;
static gg_texture_t gg_res_tex;
static gg_framebuf_t gg_res_fb;

SDL_Window *gg_window = NULL;
SDL_GLContext *gg_gl_ctx = NULL;

GLbitfield gg_buffer_bits = GL_COLOR_BUFFER_BIT;

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
        gg_buffer_bits |= GL_DEPTH_BUFFER_BIT;

        GL(glEnable(GL_DEPTH_TEST));
        GL(glDepthFunc(GL_LESS));
    }

    // window sizing/resolution
    if ((gg_static_resolution = cfg.resolution.x || cfg.resolution.y)) {
        gg_texture_make(&gg_res_tex, v2_EXPAND(cfg.resolution));
        gg_framebuf_make(&gg_res_fb, &gg_res_tex);

        gg__reset_bound_fbo();
    }

    on_resize();
}

void gg_quit(void) {
    gg_framebuf_kill(&gg_res_fb);
    gg_texture_kill(&gg_res_tex);

    SDL_GL_DeleteContext(gg_gl_ctx);
    SDL_DestroyWindow(gg_window);
}

void gg__set_bound_fbo(GLuint fbo, v2 size) {
    gg_bound_fbo = fbo;
    gg_resolution = size;
}

void gg__reset_bound_fbo(void) {
    if (gg_static_resolution) {
        gg_framebuf_bind(&gg_res_fb);
    } else {
        gg_bound_fbo = 0;
        gg_resolution = gg_window_size;
    }
}

void gg_clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
#define X(v) ((float)(v) / 255.0)
    GL(glClearColor(X(r), X(g), X(b), X(a)));
#undef X
    GL(glClear(gg_buffer_bits));
}

void gg_flip(void) {
    if (gg_static_resolution) {
        GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        gg_bound_fbo = 0;

        // scale statically sized framebuffer
        v2 dst_size = gg_window_size;
        v2 ratio = v2_div(dst_size, gg_resolution);

        ratio = v2_divs(ratio, GG_MIN(ratio.x, ratio.y));
        dst_size = v2_div(dst_size, ratio);
        dst_size = v2_MAP(dst_size, floor);

        // find centered position for framebuffer
        v2 dst_pos = v2_ZERO;

        if (dst_size.x < gg_window_size.x)
            dst_pos.x += (gg_window_size.x - dst_size.x) / 2;

        if (dst_size.y < gg_window_size.y)
            dst_pos.y += (gg_window_size.y - dst_size.y) / 2;

        // blit and flip
        gg_clear(0x0, 0x0, 0x0, 0xFF);
        gg_framebuf_blit_scaled(&gg_res_fb, dst_pos, dst_size);

        SDL_GL_SwapWindow(gg_window);

        gg__reset_bound_fbo();
    } else {
        SDL_GL_SwapWindow(gg_window);
    }
}

static void on_resize(void) {
    int width, height;

    SDL_GetWindowSize(gg_window, &width, &height);

    gg_window_size = v2_(width, height);

    if (gg_bound_fbo) {
        GL(glViewport(0, 0, v2_EXPAND(gg_resolution)));
    } else {
        gg_resolution = gg_window_size;
        GL(glViewport(0, 0, width, height));
    }
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

#ifndef GG_UTIL_H
#define GG_UTIL_H

#include <stdio.h>

/*
 * errors and debugging
 */
#define GG_ERROR(...)\
    do {\
        fprintf(stderr, "GG ERROR at %s:%d\n", __FILE__, __LINE__);\
        fprintf(stderr, __VA_ARGS__);\
        exit(-1);\
    } while (0)

#ifdef DEBUG
#define GG_ASSERT(cond, ...) if (!(cond)) GG_ERROR(__VA_ARGS__)
#else
#define GG_ASSERT(...)
#endif

// OpenGL debug macro. wrap OpenGL function calls in GL() so that errors are
// checked automatically at runtime when DEBUG is defined.
#ifdef DEBUG

static inline void gg_check_error() {
    switch (glGetError()) {
#define X(x) case x: GG_ERROR(#x "\n");
    X(GL_INVALID_ENUM);
    X(GL_INVALID_VALUE);
    X(GL_INVALID_OPERATION);
    X(GL_INVALID_FRAMEBUFFER_OPERATION);
    X(GL_OUT_OF_MEMORY);
#undef X
    default:
        return;
    }
}

#define GL(line) do { line; gg_check_error(); } while(0)
#else
#define GL(line) line
#endif

// SDL error macro
#define GG_SDL(thing) do { if (!(thing)) GG_ERROR(SDL_GetError()); } while (0)

/*
 * I/O + string bullshit
 */
char *gg_load_file(const char *filename, size_t *out_len); // malloc'd string

static inline void gg_strcpy(char *dst, const char *src) {
    while ((*dst++ = *src++))
        ;
}

#endif

#ifndef GG_UTIL_H
#define GG_UTIL_H

#include <stdio.h>

/*
 * common macros
 */

#define GG_MAX(a, b) ((a) > (b) ? (a) : (b))
#define GG_MIN(a, b) ((a) < (b) ? (a) : (b))
#define GG_ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define GG_SWAP_XOR(a, b) ((a) ^= (b) ^= (a) ^= (b))
#define GG_SWAP_TMP(a, b, tmp) do { tmp = a; a = b; b = tmp; } while (0)

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

static inline void gg_check_error(const char *file, int line) {
    switch (glGetError()) {
#define X(x) case x: fprintf(stderr, "GG ERROR at %s:%d\n%s\n", file, line, #x);
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

#define GL(line) do {\
    line;\
    gg_check_error(__FILE__, __LINE__);\
} while(0)
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

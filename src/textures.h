#ifndef GG_TEXTURES_H
#define GG_TEXTURES_H

#include <stddef.h>
#include "../gglm.h"

#include "gg.h"
#include "util.h"

typedef struct gg_texture {
    GLuint handle;
    int width, height;
} gg_texture_t;

typedef struct gg_framebuf {
    GLuint handle;
    gg_texture_t *tex;
} gg_framebuf_t;

typedef struct gg_atexture {
    v2 pos, size;
    v2 rel_pos, rel_size;
} gg_atexture_t;

void gg_texture_make(gg_texture_t *, int width, int height); // make empty
void gg_texture_load(gg_texture_t *, const char *filename); // load from file
static inline void gg_texture_kill(gg_texture_t *tex) {
    GL(glDeleteTextures(1, &tex->handle));
}

// bind to one of GL_TEXTURE0 to 31
void gg_texture_bind(gg_texture_t *, int unit);
static inline void gg_texture_unbind(void) {
    GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void gg_framebuf_make(gg_framebuf_t *, gg_texture_t *tex);
static inline void gg_framebuf_kill(gg_framebuf_t *fb) {
    GL(glDeleteFramebuffers(1, &fb->handle));
}

static inline void gg_framebuf_bind(gg_framebuf_t *fb) {
    GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->handle));
    gg__set_bound_fbo(fb->handle, v2_(fb->tex->width, fb->tex->height));
}
static inline void gg_framebuf_unbind(void) {
    GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    gg__reset_bound_fbo();
}

// blit funcs draw to currently bound draw fbo
void gg_framebuf_blit_scaled(gg_framebuf_t *fb, v2 pos, v2 size);
static inline void gg_framebuf_blit(gg_framebuf_t *fb, v2 pos) {
    gg_framebuf_blit_scaled(fb, pos, v2_(fb->tex->width, fb->tex->height));
}

// pass in an uninitialized (!!) texture to become an atlas
void gg_atlas_generate(
    gg_texture_t *atlas, const char **images, size_t num_images,
    gg_atexture_t *out_atextures
);

// malloc'd array of atextures
gg_atexture_t *gg_atexture_split(
    gg_atexture_t *, size_t rows, size_t cols, bool row_major
);

/*
 * TODO sprites, spritesheets, animations
 */

#endif

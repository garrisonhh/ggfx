#ifndef GG_2D_H
#define GG_2D_H

#include "../ggfx.h"

// 2d interface ================================================================

/*
 * pseudocode usage:
 *
 * gg2d_init() interface
 *
 * loop {
 *     // you don't need to do this every frame necessarily, but it's the most
 *     // common use case
 *     gg_clear()
 *     gg2d_blit() all atextures
 *     gg2d_draw() to actually draw the atextures
 *     gg_flip()
 *  }
 *  
 *  gg2d_quit() to be a good citizen
 */

// out_atextures should have enough space to store an atexture for each image
void gg2d_init(
    const char **images, size_t num_images, gg_atexture_t *out_atextures
);
void gg2d_quit(void);

void gg2d_blit_scaled(gg_atexture_t *atex, v2 pos, v2 size);
static inline void gg2d_blit(gg_atexture_t *atex, v2 pos) {
    gg2d_blit_scaled(atex, pos, atex->size);
}

void gg2d_draw(void); // draw batch and flush

// 2d fonts ====================================================================

// TODO also allow ttf fonts using stb_truetype

typedef struct gg2d_font {
    gg_atexture_t *atextures;
    v2 char_size;
} gg2d_font_t;

// image should be a loaded atexture for a bitmap font
void gg2d_font_make(gg2d_font_t *, gg_atexture_t *font_atex, v2 char_size);
void gg2d_font_kill(gg2d_font_t *);

void gg2d_write(gg2d_font_t *, char *text, v2 pos);

// TODO sprites!

#endif

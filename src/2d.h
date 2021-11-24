#ifndef GG_2D_H
#define GG_2D_H

#include "../ggfx.h"
#include "../gglm.h"

// 2d interface ================================================================

// out_atextures should have enough space to store an atexture for each image
void gg2d_init(
    const char **images, size_t num_images, gg_atexture_t *out_atextures
);
void gg2d_quit(void);

void gg2d_blit(gg_atexture_t *atex, v2 pos); // queue atex to batch
void gg2d_draw(void); // draw batch and flush

// 2d fonts ====================================================================
typedef struct gg2d_font {
    gg_atexture_t *atextures;
    v2 char_size;
} gg2d_font_t;

// image should be a loaded atexture for a bitmap font
void gg2d_font_make(gg2d_font_t *, gg_atexture_t *font_atex, v2 char_size);
void gg2d_font_kill(gg2d_font_t *);

void gg2d_write(gg2d_font_t *, char *text, v2 pos);

#endif

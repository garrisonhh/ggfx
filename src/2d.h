#ifndef GG_2D_H
#define GG_2D_H

#include "../ggfx.h"
#include "../gglm.h"

// out_atextures should have enough space to store an atexture for each image
void gg2d_init(
    const char **images, size_t num_images, gg_atexture_t *out_atextures
);
void gg2d_quit(void);

void gg2d_blit(gg_atexture_t *atex, v2 pos); // queue atex to batch
void gg2d_draw(void); // draw batch and flush

#endif

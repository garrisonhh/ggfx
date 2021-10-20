#ifndef GG_TEXTURES_H
#define GG_TEXTURES_H

typedef struct gg_texture {
    GLuint handle;
    int width, height;
} gg_texture_t;

void texture_make(gg_texture_t *tex, int width, int height); // empty
void texture_load(gg_texture_t *tex, const char *filename); // from file
static inline void texture_kill(gg_texture_t *tex) {
    GL(glDeleteTextures(tex->handle));
}

// bind to one of GL_TEXTURE0 to 31.
void texture_bind(gg_texture_t *tex, int unit);
static inline void texture_unbind(void) { GL(glBindTexture(GL_TEXTURE_2D, 0)); }

// submit an uninitialized texture to fill and a list of loaded textures.
// once called you can safely kill all textures in the list
void texture_fill_atlas(
    gg_texture_t *atlas, gg_texture_t *textures, size_t num_textures
);

#endif

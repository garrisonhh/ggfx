#include "../gglm.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../stb_image.h"

#include "textures.h"

typedef struct gg_atlas_rect {
    v2 pos, size;
    gg_texture_t *tex;
} gg_atlas_rect_t;

// binds texture and sets reasonable default params
static void gen_texture(gg_texture_t *tex) {
    GL(glGenTextures(1, &tex->handle));
    GL(glBindTexture(GL_TEXTURE_2D, tex->handle));

    GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

void texture_make(gg_texture_t *tex, int width, int height) {
    gen_texture(tex);

    tex->width = width;
    tex->height = height;

    GL(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, 0
    ));

    texture_unbind();
}

void texture_load(gg_texture_t *tex, const char *filename) {
    gen_texture(tex);

    char *img_bytes = stbi_load(filename, &tex->width, &tex->height, NULL, 4);

    if (!img_bytes)
        GG_ERROR("failed to load texture at \"%s\".\n", filename);

    GL(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, img_data
    ));

    stbi_image_free(img_bytes);

    texture_unbind();
}

void texture_bind(gg_texture_t *tex, int unit) {
    GG_ASSERT(unit >= 0 && unit < 32, "texture bind unit must be in 0..31.\n");

#ifdef DEBUG
    GLint bound_handle;

    GL(glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_handle));

    if (bound_handle) {
        GG_ERROR(
            "bad texture binding sanitation detected, remember to unbind "
            "textures before messing around with new textures.\n"
        );
    }
#endif

    GL(glActiveTexture(GL_TEXTURE0 + unit));
    GL(glBindTexture(GL_TEXTURE_2D, tex->handle));
}

void texture_fill_atlas(
    gg_texture_t *atlas, gg_texture_t *textures, size_t num_textures
) {
#define PUSH_RECT(rect) rects[num_rects++] = rect
#define POP_RECT() rects[--num_rects]

    /*
     * bin pack the textures
     */
    gg_atlas_rect_t *rects = calloc(sizeof(*rects) * 2 * num_textures);
    size_t num_rects = 0;

    //
    GLint max_tex_size;

    GL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size));

    gg_atlas_rect_t max_rect = { .size = v2_filled(max_tex_size) };
    PUSH_RECT(max_rect);

    for (size_t i = 0; i < num_textures; ++i) {
        ;
    }

    /*
     * blit everything over
     */

    free(rects);

#undef PUSH_RECT
#undef POP_RECT
}

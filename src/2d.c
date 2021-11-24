#include "2d.h"

// batching ====================================================================

#ifndef GG2D_BATCH_SIZE
#define GG2D_BATCH_SIZE 65536
#endif

#define GG2D_NUM_VBOS 4

static gg_texture_t gg2d_atlas;
static gg_program_t *gg2d_program;

static GLuint gg2d_vao, gg2d_vbos[GG2D_NUM_VBOS];

// uniform locations
static GLint gg2d_loc_disp_size, gg2d_loc_atlas;

static struct gg2d_batch {
    v2 src_pos[GG2D_BATCH_SIZE], src_size[GG2D_BATCH_SIZE];
    v2 dst_pos[GG2D_BATCH_SIZE], pix_size[GG2D_BATCH_SIZE];
} gg2d_batch;
static size_t gg2d_batch_idx = 0;

void gg2d_init(
    const char **images, size_t num_images, gg_atexture_t *out_atextures
) {
    gg_shader_cfg_t shader_cfg[] = {
        { "2d_vert.glsl", GG_SHADER_VERT },
        { "2d_frag.glsl", GG_SHADER_FRAG }
    };

    gg2d_program = gg_program_load(
        "gg2d-blitter", shader_cfg, GG_ARR_SIZE(shader_cfg)
    );
    gg2d_loc_disp_size = gg_program_uniform(gg2d_program, "disp_size");
    gg2d_loc_atlas = gg_program_uniform(gg2d_program, "atlas");

    if (images)
        gg_atlas_generate(&gg2d_atlas, images, num_images, out_atextures);

    GL(glGenBuffers(GG2D_NUM_VBOS, gg2d_vbos));
    GL(glGenVertexArrays(1, &gg2d_vao));
}

void gg2d_quit(void) {
    ; // TODO
}

void gg2d_blit(gg_atexture_t *atex, v2 pos) {
    GG_ASSERT(
        gg2d_batch_idx < GG2D_BATCH_SIZE,
        "ran out of gg2d batch space, please draw() more frequently or #define "
        "GG2D_BATCH_SIZE with a larger value.\n"
    );

    gg2d_batch.src_pos[gg2d_batch_idx] = atex->rel_pos;
    gg2d_batch.src_size[gg2d_batch_idx] = atex->rel_size;
    gg2d_batch.dst_pos[gg2d_batch_idx] = pos;
    gg2d_batch.pix_size[gg2d_batch_idx] = atex->size;

    ++gg2d_batch_idx;
}

/*
 * vbo_idx serves double purpose in this function of being the attribute
 * location as well as the buffer index
 */
static void gg2d_load_batch_buf(GLint vbo_idx, v2 *arr) {
    GL(glBindBuffer(GL_ARRAY_BUFFER, gg2d_vbos[vbo_idx]));
    GL(glBufferData(
        GL_ARRAY_BUFFER, gg2d_batch_idx * sizeof(*arr), arr, GL_STREAM_DRAW
    ));
    GL(glEnableVertexAttribArray(vbo_idx));
    GL(glVertexAttribPointer(vbo_idx, 2, GL_FLOAT, 0, 0, (void *)0));
    GL(glVertexAttribDivisor(vbo_idx, 1));
}

void gg2d_draw(void) {
    gg_program_bind(gg2d_program);
    gg_texture_bind(&gg2d_atlas, 0);

    GL(glBindVertexArray(gg2d_vao));

    // update uniforms
    GL(glUniform2fv(gg2d_loc_disp_size, 1, gg_resolution.ptr));
    GL(glUniform1i(gg2d_loc_atlas, 0));

    // buffer everything and draw
    gg2d_load_batch_buf(0, gg2d_batch.src_pos);
    gg2d_load_batch_buf(1, gg2d_batch.src_size);
    gg2d_load_batch_buf(2, gg2d_batch.dst_pos);
    gg2d_load_batch_buf(3, gg2d_batch.pix_size);

    GL(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, gg2d_batch_idx));
    gg2d_batch_idx = 0;

    // cleanup
    for (size_t i = 0; i < GG2D_NUM_VBOS; ++i)
        GL(glDisableVertexAttribArray(i));

    GL(glBindVertexArray(0));

    gg_texture_unbind();
    gg_program_unbind();
}

// fonts =======================================================================

#ifndef GG2D_TAB_WIDTH
#define GG2D_TAB_WIDTH (4)
#endif

void gg2d_font_make(gg2d_font_t *font, gg_atexture_t *font_atex, v2 char_size) {
    *font = (gg2d_font_t){
        .char_size = char_size
    };

    font->atextures = gg_atexture_split(font_atex, font->char_size, false);
}

void gg2d_font_kill(gg2d_font_t *font) {
    free(font->atextures);
}

void gg2d_write(gg2d_font_t *font, char *text, v2 pos) {
    v2 cursor = v2_ZERO;

    for (char *trav = text; *trav; ++trav) {
        switch (*trav) {
        case '\t':
            cursor.x += GG2D_TAB_WIDTH * font->char_size.x;

            break;
        case '\n':
            cursor.x = 0;
            cursor.y += font->char_size.y;

            break;
        default:
            gg2d_blit(&font->atextures[*trav], v2_add(pos, cursor));
            cursor.x += font->char_size.x;

            break;
        }
    }
}

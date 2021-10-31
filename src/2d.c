#include "2d.h"

#ifndef GG2D_BATCH_SIZE
#define GG2D_BATCH_SIZE 65535
#endif

#define GG2D_NUM_VBOS 3

static gg_texture_t gg2d_atlas;
static gg_program_t *gg2d_program;

static GLuint gg2d_vao, gg2d_vbos[GG2D_NUM_VBOS];
static GLint gg2d_loc_disp_size;

static struct gg2d_batch {
    v2 src_pos[GG2D_BATCH_SIZE];
    v2 dst_pos[GG2D_BATCH_SIZE];
    v2 pix_size[GG2D_BATCH_SIZE];
} gg2d_batch;
static size_t gg2d_batch_idx = 0;

void gg2d_init(
    const char **images, size_t num_images, gg_atexture_t *out_atextures
) {
    // TODO pathing here ???
    gg_shader_cfg_t shader_cfg[] = {
        { "lib/ggfx/res/2d_vert.glsl", GG_SHADER_VERT },
        { "lib/ggfx/res/2d_frag.glsl", GG_SHADER_FRAG }
    };

    gg2d_program = gg_program_load(
        "gg2d-blitter", shader_cfg, GG_ARR_SIZE(shader_cfg)
    );
    GL(gg2d_loc_disp_size = glGetUniformLocation(
        gg2d_program->handle, "disp_size"
    ));

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

    gg2d_batch.src_pos[gg2d_batch_idx] = atex->pos;
    gg2d_batch.dst_pos[gg2d_batch_idx] = pos;
    gg2d_batch.pix_size[gg2d_batch_idx] = atex->size;

    ++gg2d_batch_idx;
}


void gg2d_draw(void) {
    gg_program_bind(gg2d_program);

    GL(glBindVertexArray(gg2d_vao));

    // update uniforms
    GL(glUniform2fv(gg2d_loc_disp_size, 1, gg_window_size.ptr));

    // load everything to vertex buffers and draw
#define GG2D_LOAD_BUFFER(vbo_idx, arr) do {\
    GL(glBindBuffer(GL_ARRAY_BUFFER, gg2d_vbos[vbo_idx]));\
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_STREAM_DRAW));\
    GL(glEnableVertexAttribArray(vbo_idx));\
    GL(glVertexAttribPointer(vbo_idx, 2, GL_FLOAT, 0, 0, (void *)0));\
    GL(glVertexAttribDivisor(vbo_idx, 4));\
} while (0)

    GG2D_LOAD_BUFFER(0, gg2d_batch.src_pos);
    GG2D_LOAD_BUFFER(1, gg2d_batch.dst_pos);
    GG2D_LOAD_BUFFER(2, gg2d_batch.pix_size);

#undef GG2D_LOAD_BUFFER

    GL(glDrawArraysInstanced(
        GL_TRIANGLE_STRIP, 0, 4, gg2d_batch_idx
    ));

    // cleanup
    for (size_t i = 0; i < GG2D_NUM_VBOS; ++i)
        GL(glDisableVertexAttribArray(i));

    GL(glBindVertexArray(0));

    gg2d_batch_idx = 0;
}

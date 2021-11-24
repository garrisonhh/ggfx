#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../stb_image.h"

#include "../ggfx.h"

// individual textures =========================================================

// binds texture and sets reasonable default params
static void gen_texture(gg_texture_t *tex) {
    GL(glGenTextures(1, &tex->handle));
    GL(glBindTexture(GL_TEXTURE_2D, tex->handle));

    GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

void gg_texture_make(gg_texture_t *tex, int width, int height) {
    gen_texture(tex);

    tex->width = width;
    tex->height = height;

    GL(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, 0
    ));

    gg_texture_unbind();
}

void gg_texture_load(gg_texture_t *tex, const char *filename) {
    gen_texture(tex);

    unsigned char *img_bytes = stbi_load(
        filename, &tex->width, &tex->height, NULL, 4
    );

    if (!img_bytes)
        GG_ERROR("failed to load texture at \"%s\".\n", filename);

    GL(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, tex->width, tex->height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, img_bytes
    ));

    stbi_image_free(img_bytes);

    gg_texture_unbind();
}

void gg_texture_bind(gg_texture_t *tex, int unit) {
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

// framebuffers ================================================================

void gg_framebuf_make(gg_framebuf_t *fb, gg_texture_t *tex) {
    fb->tex = tex;

    GL(glGenFramebuffers(1, &fb->handle));
    GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->handle));
    GL(glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->handle, 0
    ));
    GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
}

void gg_framebuf_blit_scaled(gg_framebuf_t *fb, v2 pos, v2 size) {
    v2 bottom_right = v2_add(pos, size);

    GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->handle));
    GL(glBlitFramebuffer(
        0.0, 0.0, fb->tex->width, fb->tex->height,
        v2_EXPAND(pos), v2_EXPAND(bottom_right),
        gg_buffer_bits, GL_NEAREST
    ));
    GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
}

// texture atlassing ===========================================================

#ifndef GG_ATLAS_MAX_IMAGES
#define GG_ATLAS_MAX_IMAGES 256
#endif

typedef struct gg_atlas_ctx {
    gg_texture_t textures[GG_ATLAS_MAX_IMAGES];
    gg_atexture_t tex_heap[GG_ATLAS_MAX_IMAGES];
    size_t heap_size;
} gg_atlas_ctx_t;

static inline float atlas_score(gg_atexture_t rect) {
    return GG_MAX(rect.pos.x, rect.pos.y);
}

// for sorting the heap post-deletion
static void atlas_heapify_topdown(gg_atlas_ctx_t *ctx, size_t index) {
    size_t best = index;
    size_t left = index * 2 + 1;
    size_t right = left + 1;

    if (left < ctx->heap_size
     && atlas_score(ctx->tex_heap[left]) < atlas_score(ctx->tex_heap[best]))
        best = left;

    if (right < ctx->heap_size
     && atlas_score(ctx->tex_heap[right]) < atlas_score(ctx->tex_heap[best]))
        best = right;

    if (best != index) {
        gg_atexture_t tmp;
        GG_SWAP_TMP(ctx->tex_heap[index], ctx->tex_heap[best], tmp);

        atlas_heapify_topdown(ctx, best);
    }
}

// for sorting the heap post-insertion
static void atlas_heapify_bottomup(gg_atlas_ctx_t *ctx, size_t index) {
    if (index) {
        size_t parent = (index - 1) >> 1;

        if (atlas_score(ctx->tex_heap[index])
            < atlas_score(ctx->tex_heap[parent])) {
            gg_atexture_t tmp;
            GG_SWAP_TMP(ctx->tex_heap[index], ctx->tex_heap[parent], tmp);

            atlas_heapify_topdown(ctx, parent);
        }
    }
}

static inline void atlas_del(gg_atlas_ctx_t *ctx, size_t index) {
    ctx->tex_heap[index] = ctx->tex_heap[--ctx->heap_size];

    if (index < ctx->heap_size)
        atlas_heapify_topdown(ctx, index);
}

static inline void atlas_add(gg_atlas_ctx_t *ctx, gg_atexture_t rect) {
    ctx->tex_heap[ctx->heap_size] = rect;
    atlas_heapify_bottomup(ctx, ctx->heap_size++);
}

void gg_atlas_generate(
    gg_texture_t *atlas, const char **image_paths, size_t num_images,
    gg_atexture_t *out_atextures
) {
    GG_ASSERT(
        num_images <= GG_ATLAS_MAX_IMAGES,
        "submitted too many images to gg_atlas_generate. please #define "
        "GG_ATLAS_MAX_IMAGES with a higher value.\n"
    );
    GG_ASSERT(out_atextures, "atlas_generate requires the out parameter.\n");

    // init + load
    gg_atlas_ctx_t ctx;

    for (size_t i = 0; i < num_images; ++i)
        gg_texture_load(&ctx.textures[i], image_paths[i]);

    // generate data for packed rects
    GLint max_tex_size;
    size_t num_final = 0;
    v2 atlas_size = v2_ZERO;

    GL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size));

    ctx.tex_heap[0] = (gg_atexture_t){ .size = v2_fill(max_tex_size) };
    ctx.heap_size = 1;

    for (size_t i = 0; i < num_images; ++i) {
        gg_texture_t *tex = &ctx.textures[i];

        // find first rect in heap that fits tex
        bool found = false;
        size_t best_fit;

        for (size_t i = 0; i < ctx.heap_size; ++i) {
            if (ctx.tex_heap[i].size.x >= tex->width
             && ctx.tex_heap[i].size.y >= tex->height) {
                found = true;
                best_fit = i;
                break;
            }
        }

        if (!found)
            GG_ERROR("ran out of atlas space! must use a second atlas.\n");

        // finalize rect
        gg_atexture_t old = ctx.tex_heap[best_fit];

        atlas_del(&ctx, best_fit);

        out_atextures[num_final++] = (gg_atexture_t){
            .pos = old.pos,
            .size = v2_(tex->width, tex->height)
        };

        atlas_size.x = GG_MAX(atlas_size.x, old.pos.x + tex->width);
        atlas_size.y = GG_MAX(atlas_size.y, old.pos.y + tex->height);

        // sort back in new rects created with the leftovers of the old rect
        gg_atexture_t up, right;
        float up_margin = old.size.y - tex->height;
        float right_margin = old.size.x - tex->width;

        up.pos = right.pos = old.pos;
        up.pos.y += tex->height;
        right.pos.x += tex->width;

        up.size = v2_(tex->width, up_margin);
        right.size = v2_(right_margin, tex->height);

        if (right_margin > up_margin)
            right.size.y = old.size.y;
        else
            up.size.x = old.size.x;

        if (GG_MIN(up.size.x, up.size.y) > 0)
            atlas_add(&ctx, up);
        if (GG_MIN(right.size.x, right.size.y) > 0)
            atlas_add(&ctx, right);
    }

    // generate relative coordinates for atextures now that atlas size is known
    for (size_t i = 0; i < num_images; ++i) {
        gg_atexture_t *atex = &out_atextures[i];

        atex->rel_pos = v2_div(atex->pos, atlas_size);
        atex->rel_size = v2_div(atex->size, atlas_size);
    }

    // blit textures
    gg_framebuf_t atlas_fb, tex_fb;

    gg_texture_make(atlas, v2_EXPAND(atlas_size));

    gg_framebuf_make(&atlas_fb, atlas);
    gg_framebuf_bind(&atlas_fb);

    for (size_t i = 0; i < num_images; ++i) {
        gg_framebuf_make(&tex_fb, &ctx.textures[i]);
        gg_framebuf_blit(&tex_fb, out_atextures[i].pos);
        gg_framebuf_kill(&tex_fb);
    }

    gg_framebuf_unbind();

    // clean up
    for (size_t i = 0; i < num_images; ++i)
        gg_texture_kill(&ctx.textures[i]);
}

// splits atexture by cell size (in pix)
gg_atexture_t *gg_atexture_split(
    gg_atexture_t *atex, v2 cell_size, bool row_major
) {
    v2 grid_size = v2_div(atex->size, cell_size);

    return gg_atexture_split_grid(atex, grid_size.x, grid_size.y, row_major);
}

// splits a single atexture into a grid of atextures
gg_atexture_t *gg_atexture_split_grid(
    gg_atexture_t *atex, size_t cols, size_t rows, bool row_major
) {
    v2 split_cell = v2_div(v2_fill(1.0), v2_(cols, rows));
    gg_atexture_t *arr = malloc(rows * cols * sizeof(*arr));

    for (size_t col = 0; col < cols; ++col) {
        for (size_t row = 0; row < rows; ++row) {
            v2 split_pos = v2_mul(v2_(col, row), split_cell);
            size_t index = row_major
                ? row + col * rows
                : col + row * cols;

            arr[index] = (gg_atexture_t){
                .pos = v2_add(atex->pos, v2_mul(split_pos, atex->size)),
                .size = v2_mul(split_cell, atex->size),
                .rel_pos = v2_add(
                    atex->rel_pos,
                    v2_mul(split_pos, atex->rel_size)
                ),
                .rel_size = v2_mul(split_cell, atex->rel_size)
            };
        }
    }

    return arr;
}

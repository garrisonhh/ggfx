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

void gg_framebuf_blit(gg_framebuf_t *fb, v2 pos) {
    GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->handle));
    GL(glBlitFramebuffer(
        0.0, 0.0, fb->tex->width, fb->tex->height,
        v2_EXPAND(pos), pos.x + fb->tex->width, pos.y + fb->tex->height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST
    ));
    GL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
}

// texture atlassing ===========================================================

#ifndef GG_ATLAS_MAX_IMAGES
#define GG_ATLAS_MAX_IMAGES 128
#endif

typedef struct gg_atlas_rect { v2 pos, size; } gg_atlas_rect_t;
typedef struct gg_atlas_final { v2 pos; gg_texture_t *tex; } gg_atlas_final_t;

typedef struct gg_atlas_ctx {
    gg_atlas_rect_t tex_heap[GG_ATLAS_MAX_IMAGES];
    gg_texture_t textures[GG_ATLAS_MAX_IMAGES];
    gg_atlas_final_t tex_final[GG_ATLAS_MAX_IMAGES];
    size_t heap_size, num_textures, num_final;
} gg_atlas_ctx_t;

static inline float atlas_score(gg_atlas_rect_t rect) {
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
        gg_atlas_rect_t tmp;
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
            gg_atlas_rect_t tmp;
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

static inline void atlas_add(gg_atlas_ctx_t *ctx, gg_atlas_rect_t rect) {
    ctx->tex_heap[ctx->heap_size] = rect;
    atlas_heapify_bottomup(ctx, ctx->heap_size++);
}

// for initial texture sort
int gg_atlas_texture_cmp(const void *a, const void *b) {
    const gg_texture_t *t1 = a, *t2 = b;

    return GG_MAX(t2->width, t2->height) - GG_MAX(t1->width, t1->height);
}

void gg_atlas_generate(
    gg_texture_t *atlas, const char **image_paths, size_t num_images
) {
    GG_ASSERT(
        num_images <= GG_ATLAS_MAX_IMAGES,
        "submitted too many images to gg_atlas_generate. please #define "
        "GG_ATLAS_MAX_IMAGES with a higher value.\n"
    );

    // init + load
    gg_atlas_ctx_t ctx;

    ctx.num_final = 0;
    ctx.num_textures = num_images;

    for (size_t i = 0; i < num_images; ++i)
        gg_texture_load(&ctx.textures[i], image_paths[i]);

    // sorting by max dimension (not necessary, but gives better results)
    qsort(
        ctx.textures, ctx.num_textures, sizeof(*ctx.textures),
        gg_atlas_texture_cmp
    );

    // generate data for packed rects
    GLint max_tex_size;
    GL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size));

    v2 atlas_size = v2_ZERO;

    ctx.tex_heap[0] = (gg_atlas_rect_t){ .size = v2_fill(max_tex_size) };
    ctx.heap_size = 1;

    for (size_t i = 0; i < ctx.num_textures; ++i) {
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
        gg_atlas_rect_t old = ctx.tex_heap[best_fit];

        atlas_del(&ctx, best_fit);

        ctx.tex_final[ctx.num_final++] = (gg_atlas_final_t){ old.pos, tex };

        atlas_size.x = GG_MAX(atlas_size.x, old.pos.x + tex->width);
        atlas_size.y = GG_MAX(atlas_size.y, old.pos.y + tex->height);

        // sort back in new rects created with the leftovers of the old rect
        gg_atlas_rect_t up, right;
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

    // blit textures
    gg_framebuf_t atlas_fb, tex_fb;

    gg_texture_make(atlas, v2_EXPAND(atlas_size));

    gg_framebuf_make(&atlas_fb, atlas);
    gg_framebuf_bind(&atlas_fb);

    for (size_t i = 0; i < ctx.num_final; ++i) {
        gg_atlas_final_t *final = &ctx.tex_final[i];

        gg_framebuf_make(&tex_fb, final->tex);
        gg_framebuf_blit(&tex_fb, final->pos);
        gg_framebuf_kill(&tex_fb);
    }

    gg_framebuf_unbind();

    // clean up
    for (size_t i = 0; i < ctx.num_textures; ++i)
        gg_texture_kill(&ctx.textures[i]);
}


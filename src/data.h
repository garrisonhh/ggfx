#ifndef GG_DATA_H
#define GG_DATA_H

#include <stdint.h>
#include <stddef.h>

// page-based monotonic allocator
typedef struct gg_pages {
    uint8_t **pages;
    size_t cur_page, pages_cap;
    size_t cur_size, page_bytes;

#ifdef DEBUG
    size_t large_allocs, total_allocs;
#endif
} gg_pages_t;

void gg_pages_make(gg_pages_t *, size_t page_bytes);
void gg_pages_kill(gg_pages_t *);

void *gg_pages_alloc(gg_pages_t *, size_t bytes);

#ifdef DEBUG
static inline void gg_pages_debug(gg_pages_t *mem) {
    double page_density = (double)mem->total_allocs
                        / (double)(mem->cur_page + 1);
    double large_percent = (double)mem->large_allocs
                         / (double)mem->total_allocs;

    printf(
        "=== page debug info ===\n"
        "allocs:       %zu\n"
        "pages:        %zu\n"
        "allocs/page:  %.2lf\n"
        "large/allocs: %.2lf\n",
        mem->total_allocs, mem->cur_page + 1, page_density, large_percent
    );
}
#else
#define gg_pages_debug(...)
#endif

#endif

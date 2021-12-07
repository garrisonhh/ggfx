#include <stdio.h>
#include <stdlib.h>

#include "data.h"

void gg_pages_make(gg_pages_t *mem, size_t page_bytes) {
#ifndef GG_PAGES_INIT_CAP
#define GG_PAGES_INIT_CAP 8
#endif
    *mem = (gg_pages_t){0};

    mem->page_bytes = page_bytes;
    mem->pages_cap = GG_PAGES_INIT_CAP;
    mem->pages = malloc(mem->pages_cap * sizeof(*mem->pages));
    mem->pages[0] = malloc(mem->page_bytes);
}

void gg_pages_kill(gg_pages_t *mem) {
    for (size_t i = 0; i <= mem->cur_page; ++i)
        free(mem->pages[i]);

    free(mem->pages);
}

// allocates a new page and moves cur_page, returning the new page
static void *gg_next_page(gg_pages_t *mem, size_t bytes) {
    if (++mem->cur_page == mem->pages_cap) {
        mem->pages_cap *= 2;
        mem->pages = realloc(mem->pages, mem->pages_cap * sizeof(*mem->pages));
    }

    void *page = mem->pages[mem->cur_page] = malloc(bytes);

    if (!page)
        GG_ERROR("page allocation failed!\n");

    return page;
}

void *gg_pages_alloc(gg_pages_t *mem, size_t bytes) {
    void *ptr;

    if (mem->cur_size + bytes > mem->page_bytes) {
        // new page
        if (bytes > mem->page_bytes) {
            // large allocation (gets a unique page)
            uint8_t *last_page = mem->pages[mem->cur_page];

            ptr = gg_next_page(mem, bytes);

            mem->pages[mem->cur_page - 1] = ptr;
            mem->pages[mem->cur_page] = last_page;

#ifdef DEBUG
            ++mem->large_allocs;
#endif
        } else {
            ptr = gg_next_page(mem, mem->page_bytes);
            mem->cur_size = 0;
        }
    } else {
        // allocate from page
        ptr = mem->pages[mem->cur_page] + mem->cur_size;
        mem->cur_size += bytes;
    }

#ifdef DEBUG
    ++mem->total_allocs;
#endif

    return ptr;
}

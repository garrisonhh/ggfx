#include <libghh/ghh.h>

#include "events.h"

typedef struct gg_event {
    SDL_EventType base_type;
    uint32_t sub_type;
} gg_event_t;

static hmap_t gg_events; // map of gg_event => gg_event_hook

void gg_events_init(void) {
    // ensure struct packing won't break hashing
    gg_event_t test_ev;

    if (sizeof(test_ev) != sizeof(test_ev.base_type) + sizeof(test_ev.sub_type))
        GG_ERROR("gg_event_t packing failed.");

    hmap_make(&gg_events);
}

void gg_events_quit(void) {
    hmap_kill(&gg_events);
}

void gg_events_map(
    SDL_EventType base_type, uint32_t sub_type, gg_event_hook_t hook
) {
    gg_event_t gg_event = {
        .base_type = base_type,
        .sub_type = sub_type
    };

    hmap_put(&gg_events, gg_event, hook);
}

void gg_events_process(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        // TODO REMOVE THIS
        if (event.type == SDL_QUIT)
            exit(0);

        gg_event_t gg_event = {
            .base_type = event.type,
            .sub_type = event.common.type
        };

        gg_event_hook_t hook = hmap_get(&gg_events, gg_event);

        if (hook)
            hook(&event);
    }
}

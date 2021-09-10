#include <stdbool.h>
#include <libghh/ghh.h>

#include "events.h"

typedef struct gg_event {
    SDL_EventType type; // 0 (aka SDL_FIRSTEVENT) if unmapped
    gg_event_hook_t hook;
} gg_event_t;

#define GG_NUM_EVENTS (256)
static gg_event_t gg_events[GG_NUM_EVENTS];

// adds an event hook mapping
void gg_events_hook(SDL_EventType event_type, gg_event_hook_t hook) {
    size_t index = hash_any(event_type) % GG_NUM_EVENTS;

    // it's impossible to register 256 SDL_EventTypes, so this won't break
    while (gg_events[index].type)
        index = (index + 1) % GG_NUM_EVENTS;

    gg_events[index] = (gg_event_t){event_type, hook};
}

// retrieves an event hook mapping
static gg_event_hook_t gg_events_get_hook(SDL_EventType event_type) {
    size_t index = hash_any(event_type) % GG_NUM_EVENTS;

    while (gg_events[index].type) {
        if (gg_events[index].type == event_type)
            return gg_events[index].hook;

        index = (index + 1) % GG_NUM_EVENTS;
    }

    return NULL;
}

// polls events and calls hooks
void gg_events_process(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        // use atexit() to define an exit function
        if (event.type == SDL_QUIT)
            exit(0);

        gg_event_hook_t hook = gg_events_get_hook(event.type);

        if (hook)
            hook(&event);
    }
}

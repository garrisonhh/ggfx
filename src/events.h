#ifndef GG_EVENTS_H
#define GG_EVENTS_H

/*
 * events.* provides event hook management.
 */

#include <SDL2/SDL.h>

typedef void (*gg_event_hook_t)(SDL_Event *);

// adds a hook which will be called whenever the specified event type appears
void gg_events_hook(SDL_EventType, gg_event_hook_t);

// polls SDL events
void gg_events_process(void);

#endif

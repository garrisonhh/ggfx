#ifndef GG_EVENTS_H
#define GG_EVENTS_H

/*
 * events.* provides event hook management.
 */

#include <SDL2/SDL.h>

typedef void (*gg_event_hook_t)(SDL_Event *);

void gg_events_init(void);
void gg_events_quit(void);

void gg_events_map(SDL_EventType base_type, uint32_t sub_type, gg_event_hook_t);

void gg_events_process(void);

#endif

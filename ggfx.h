#ifndef GGFX_H
#define GGFX_H

/*

ggfx is a header-only data-oriented minimal-cost abstraction over OpenGL 3.3
and SDL2 made for making fun low-level graphics code easier without sacrificing
on overhead.

unlike many header-only libraries, ggfx does rely on some dependencies:
- SDL2 linked so that #include <SDL2/SDL.h> works
- libghh linked so that #include <libghh/ghh.h> works
  - also expects GHH_IMPL is defined somewhere else

planned features:
- minimal programmer memory management
- an event management system to remove SDL2 boilerplate
- shaders
  - automatic batching
- texture atlases
  - automatic bin packing

*/

#ifdef GGFX_IMPL
#define GLAD_IMPL
#endif

#include "glad/glad.h"
#include "src/gg.h"
#include "src/events.h"

#ifdef GGFX_IMPL
#include "src/gg.c"
#include "src/events.c"
#endif

#endif

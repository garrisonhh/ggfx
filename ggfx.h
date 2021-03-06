#ifndef GGFX_H
#define GGFX_H

/*
 * ggfx is a header-only data-oriented minimal-cost abstraction over OpenGL 3.3
 * and SDL2 made for making fun low-level graphics code easier without adding
 * significant overhead.
 * 
 * unlike many header-only libraries, ggfx does rely on some dependencies:
 * - libghh is included as part of the library
 * - SDL2 linked so that #include <SDL2/SDL.h> works
 * 
 * planned features: TODO this is out of date
 * - minimal programmer memory management
 * - shaders
 *   - automatic batching
 * - texture atlases
 *   - automatic bin packing
 */

#ifdef GGFX_IMPL
#define GLAD_IMPL
#endif

#ifndef GHH_IMPL
#define GHH_IMPL
#endif

#include "lib/gglm/gglm.h"
#include "lib/glad/glad.h"
#include "lib/libghh/ghh.h"

#include "src/util.h"
#include "src/gg.h"
#include "src/shaders.h"
#include "src/textures.h"
#include "src/2d.h"

#ifdef GGFX_IMPL
#include "src/gg.c"
#include "src/shaders.c"
#include "src/textures.c"
#include "src/2d.c"
#include "src/util.c"
#endif

#endif

#ifndef GHH_GL_H
#define GHH_GL_H

/*

ghh_gl is a header-only data-oriented minimal-cost abstraction over OpenGL 3.3.

planned features:
- minimal programmer memory management
- shaders
  - automatic batching
- texture atlases
  - automatic bin packing

*/

#ifdef GG_IMPL
#define GLAD_IMPL
#include "glad/glad.h"

#include "src/gg.c"
#endif

#include "src/gg.h"

#endif // GHH_GFX_H

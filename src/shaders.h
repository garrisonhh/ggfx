#ifndef GG_SHADERS_H
#define GG_SHADERS_H

#include <stddef.h>

#include "../ggfx.h"

#define GG_SHADER_TYPES() \
	X(GG_SHADER_VERT, GL_VERTEX_SHADER)\
	X(GG_SHADER_GEOM, GL_GEOMETRY_SHADER)\
	X(GG_SHADER_FRAG, GL_FRAGMENT_SHADER)

typedef enum gg_shader_types {
#define X(a, b) a,
	GG_SHADER_TYPES()
#undef X
	GG_NUM_SHADER_TYPES
} gg_shader_e;

typedef struct gg_shader {
    GLuint handle;
    GLenum type;
} gg_shader_t;

typedef struct gg_shader_cfg {
    const char *filename;
    gg_shader_e type;
} gg_shader_cfg_t;

typedef struct gg_program {
    char name[80];

    gg_shader_t shaders[GG_NUM_SHADER_TYPES];
    size_t num_shaders;

    GLuint handle;
} gg_program_t;

gg_program_t *gg_program_load(
    const char *name, gg_shader_cfg_t *shaders, size_t num_shaders
);

static inline void gg_program_bind(gg_program_t *program) {
    GL(glUseProgram(program->handle));
}
static inline void gg_program_unbind(void) {
    GL(glUseProgram(0));
}
static inline GLint gg_program_uniform(gg_program_t *program, const char *name) {
    GLint loc;
    GL(loc = glGetUniformLocation(program->handle, name));
    return loc;
}

#endif


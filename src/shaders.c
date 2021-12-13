#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../ggfx.h"

#ifndef GG_MAX_SHADERS
#define GG_MAX_SHADERS 128
#endif

#ifndef GG_MAX_PROGRAMS
#define GG_MAX_PROGRAMS 32
#endif

static GLenum GG_GL_SHADER_TYPES[] = {
#define X(a, b) b,
	GG_SHADER_TYPES()
#undef X
};

static void load_shader(gg_shader_t *shader, gg_shader_cfg_t *cfg) {
    // load source from file
    size_t len_source;
    char *source = gg_load_file(cfg->filename, &len_source);
    GLint len_source_int = len_source;

    GL(shader->handle = glCreateShader(GG_GL_SHADER_TYPES[cfg->type]));
    GL(glShaderSource(
        shader->handle, 1, (const char * const *)&source, &len_source_int
    ));

    // compile shader and check for errors
    GLint success;

    GL(glCompileShader(shader->handle));
    GL(glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &success));

    if (!success) {
        char err_buf[1024];

        GL(glGetShaderInfoLog(shader->handle, sizeof(err_buf), NULL, err_buf));

        // TODO do some introspection on source string maybe?
        fprintf(
            stderr, "GG ERROR in compiling shader at \"%s\":\n%s",
            cfg->filename, err_buf
        );
        exit(-1);
    }

    free(source);
}

static void check_program(gg_program_t *program, GLuint param) {
    GLint success;

    GL(glGetProgramiv(program->handle, param, &success));

    if (!success) {
        char err_buf[1024];

        GL(glGetProgramInfoLog(
            program->handle, sizeof(err_buf), NULL, err_buf
        ));

        fprintf(
            stderr, "GG ERROR in compiling program \"%s\":\n%s",
            program->name, err_buf
        );
        exit(-1);
    }
}

gg_program_t *gg_program_load(
    const char *name, gg_shader_cfg_t *configs, size_t num_shaders
) {
#ifdef DEBUG
    bool types[GG_NUM_SHADER_TYPES] = {0};

    for (size_t i = 0; i < num_shaders; ++i) {
        if (types[configs[i].type])
            GG_ERROR("program \"%s\" has a repeated shader type.\n", name);
        else
            types[configs[i].type] = true;
    }
#endif

    gg_program_t *program = mono_alloc(&gg_pool, sizeof(*program));

    *program = (gg_program_t){0};

    gg_strcpy(program->name, name);
    GL(program->handle = glCreateProgram());

    for (size_t i = 0; i < num_shaders; ++i) {
        gg_shader_t *shader = &program->shaders[program->num_shaders++];

        load_shader(shader, &configs[i]);
        GL(glAttachShader(program->handle, shader->handle));
    }

    GL(glLinkProgram(program->handle));
    check_program(program, GL_LINK_STATUS);

    GL(glValidateProgram(program->handle));
    check_program(program, GL_VALIDATE_STATUS);

    return program;
}

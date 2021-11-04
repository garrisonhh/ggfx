#version 330 core

in vec2 v_src_pos; // in pixels

layout (location = 0) out vec4 frag_color;

uniform sampler2D atlas;

void main() {
    vec2 atlas_size = vec2(textureSize(atlas, 0)); // could do this in atlas gen

    frag_color = texture(atlas, v_src_pos / atlas_size);
}

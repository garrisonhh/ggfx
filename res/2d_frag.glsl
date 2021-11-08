#version 330 core

in vec2 v_src_pos;

layout (location = 0) out vec4 frag_color;

uniform sampler2D atlas;

void main() {
    frag_color = texture(atlas, v_src_pos);
}

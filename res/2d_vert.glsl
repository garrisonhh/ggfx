#version 330 core

vec2 quad[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0)
);

layout (location = 0) in vec2 src_pos;
layout (location = 1) in vec2 dst_pos;
layout (location = 2) in vec2 pix_size;

out vec2 v_src_pos;

uniform vec2 disp_size;

void main() {
    // pixel location form topleft
    vec2 pos = dst_pos + pix_size * quad[gl_VertexID];

    // pixel location -> opengl coords
    pos /= disp_size * 0.5;
    pos -= vec2(1.0);
    pos.y = -pos.y;

    gl_Position = vec4(pos, 0.0, 1.0);
    
    // atlas quad position
    v_src_pos = src_pos + pix_size * quad[gl_VertexID];
}

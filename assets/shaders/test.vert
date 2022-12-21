#version 450

#include <shaders/test.glsl>

void main() {
    gl_Position = vec4(POSITIONS[gl_VertexIndex], 1);
}

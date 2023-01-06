#version 450

#include <uniform_renderer.glsl>

layout(location = 0) in vec3 aPosition;

layout(location = 0) out vec3 vDirection;

void main() {
    mat4 view = uView;
    view[3] = vec4(0, 0, 0, 1);
    vec4 position = uProjection * view * vec4(aPosition, 1);
    gl_Position = position.xyww;
    vDirection = aPosition;
}

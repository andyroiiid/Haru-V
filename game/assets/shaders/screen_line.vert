#version 450

#include <uniform_renderer.glsl>

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

layout (location = 0) out vec4 vColor;

layout (push_constant) uniform PushConstantData {
    vec2 uP0;
    vec2 uP1;
    vec4 uColor;
};

void main() {
    vec2 position = mix(uP0, uP1, aPosition) * uScreenInfo.zw * 2.0 - 1.0;
    gl_Position = vec4(position, 0.0, 1.0);
    vColor = uColor;
}

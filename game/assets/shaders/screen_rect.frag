#version 450

layout (location = 0) in vec2 vTexCoord;
layout (location = 1) in vec4 vColor;

layout (location = 0) out vec4 fColor;

layout (set = 1, binding = 0) uniform sampler2D uTexture;

void main() {
    fColor = texture(uTexture, vTexCoord) * vColor;
}

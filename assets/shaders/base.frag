#version 450

#include <shaders/utils.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec4 fWorldPositionDiffuse;
layout (location = 1) out vec4 fWorldNormal;

layout (set = 1, binding = 0) uniform sampler2D uTexture;

void main() {
    vec4 diffuse = texture(uTexture, vTexCoord);
    float packedDiffuse = pack(diffuse);
    fWorldPositionDiffuse = vec4(vWorldPosition, packedDiffuse);
    fWorldNormal = vec4(normalize(vWorldNormal), 0);
}

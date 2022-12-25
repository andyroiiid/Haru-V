#version 450

#include <shaders/utils.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec4 fWorldPositionDiffuse;
layout (location = 1) out vec4 fWorldNormalMRA;

layout (set = 1, binding = 0) uniform sampler2D uTexture;

void main() {
    float packedDiffuse = pack(texture(uTexture, vTexCoord));
    fWorldPositionDiffuse = vec4(vWorldPosition, packedDiffuse);
    float packedMRA = pack(vec4(0.0, 0.5, 1.0, 0.0));
    fWorldNormalMRA = vec4(vWorldNormal, packedMRA);
}

#version 450

#include <shaders/utils.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec4 fWorldPositionAlbedo;
layout (location = 1) out vec4 fWorldNormalMRA;
layout (location = 2) out vec4 fEmissive;

layout (set = 1, binding = 0) uniform sampler2D uAlbedo;
layout (set = 1, binding = 1) uniform sampler2D uNormal;
layout (set = 1, binding = 2) uniform sampler2D uMRA;
layout (set = 1, binding = 3) uniform sampler2D uEmissive;

void main() {
    float packedAlbedo = pack(texture(uAlbedo, vTexCoord));
    fWorldPositionAlbedo = vec4(vWorldPosition, packedAlbedo);
    float packedMRA = pack(texture(uMRA, vTexCoord));
    fWorldNormalMRA = vec4(vWorldNormal, packedMRA);
    fEmissive = texture(uEmissive, vTexCoord);
}

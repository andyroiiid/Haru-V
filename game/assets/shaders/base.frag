#version 450

#include <shaders/utils.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec4 fWorldPositionMetallic;
layout (location = 1) out vec4 fWorldNormalRoughness;
layout (location = 2) out vec4 fAlbedoAmbientOcclusion;
layout (location = 3) out vec4 fEmissive;

layout (set = 1, binding = 0) uniform sampler2D uAlbedo;
layout (set = 1, binding = 1) uniform sampler2D uNormal;
layout (set = 1, binding = 2) uniform sampler2D uMRA;
layout (set = 1, binding = 3) uniform sampler2D uEmissive;

void main() {
    const vec3 albedo = texture(uAlbedo, vTexCoord).rgb;
    const vec3 mra = texture(uMRA, vTexCoord).rgb;
    const float metallic = mra.r;
    const float roughness = mra.g;
    const float ambientOcclusion = mra.b;
    fWorldPositionMetallic = vec4(vWorldPosition, metallic);
    fWorldNormalRoughness = vec4(vWorldNormal, roughness);
    fAlbedoAmbientOcclusion = vec4(albedo, ambientOcclusion);
    fEmissive = texture(uEmissive, vTexCoord);
}

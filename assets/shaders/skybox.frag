#version 450

#include <shaders/utils.glsl>

layout (location = 0) in vec3 vDirection;

layout (location = 0) out vec4 fWorldPositionMetallic;
layout (location = 1) out vec4 fWorldNormalRoughness;
layout (location = 2) out vec4 fAlbedoAmbientOcclusion;
layout (location = 3) out vec4 fEmissive;

layout (set = 1, binding = 0) uniform sampler2D uBrdfLut;
layout (set = 1, binding = 1) uniform sampler2D uSkybox;
layout (set = 1, binding = 2) uniform sampler2D uSkyboxSpecular;
layout (set = 1, binding = 3) uniform sampler2D uSkyboxIrradiance;

void main() {
    vec3 direction = normalize(vDirection);
    fWorldPositionMetallic = vec4(sign(vDirection) * INF, 0.0);
    fWorldNormalRoughness = vec4(-direction, 1.0);
    fAlbedoAmbientOcclusion = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 color = texture(uSkybox, SampleSphericalMap(direction)).rgb;
    fEmissive = vec4(color, 1.0);
}

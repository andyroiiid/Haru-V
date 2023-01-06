#version 450

#include <utils.glsl>

#define IBL_TEXTURE_SET 1
#include <ibl.glsl>

layout (location = 0) in vec3 vDirection;

layout (location = 0) out vec4 fWorldPositionMetallic;
layout (location = 1) out vec4 fWorldNormalRoughness;
layout (location = 2) out vec4 fAlbedoAmbientOcclusion;
layout (location = 3) out vec4 fEmissive;

void main() {
    vec3 direction = normalize(vDirection);
    fWorldPositionMetallic = vec4(sign(vDirection) * INF, 0.0);
    fWorldNormalRoughness = vec4(-direction, 1.0);
    fAlbedoAmbientOcclusion = vec4(0.0, 0.0, 0.0, 0.0);
    fEmissive = RGBMToLinear(texture(uSkybox, SampleSphericalMap(direction)));
}

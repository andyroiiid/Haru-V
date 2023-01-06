#ifndef CSM_GLSL
#define CSM_GLSL

#include <shaders/uniform_lighting.glsl>

layout (set = CSM_TEXTURE_SET, binding = 0) uniform sampler2DArrayShadow uShadowMap;

float ShadowPCF(vec3 shadowCoord, int layer) {
    const vec2 texelSize = 1.0 / textureSize(uShadowMap, 0).xy;
    float shadow = 0.0;
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            vec4 coord = vec4(shadowCoord.xy + vec2(x, y) * texelSize, layer, shadowCoord.z);
            shadow += texture(uShadowMap, coord);
        }
    }
    shadow /= 25.0;
    return shadow;
}

int GetShadowLayer(vec4 viewSpacePos)
{
    const float depth = viewSpacePos.z;
    int layer = 0;
    if (depth > uCascadeShadowMapSplits[0]) layer = 1;
    if (depth > uCascadeShadowMapSplits[1]) layer = 2;
    if (depth > uCascadeShadowMapSplits[2]) layer = 3;
    return layer;
}

float ReadShadowMap(vec4 viewSpacePos, vec4 worldPos)
{
    int layer = GetShadowLayer(viewSpacePos);
    vec4 shadowCoord = uShadowMatrices[layer] * worldPos;
    shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
    shadowCoord /= shadowCoord.w;
    return shadowCoord.z <= -1.0 || shadowCoord.z >= 1.0 ? 0.0 : ShadowPCF(shadowCoord.xyz, layer);
}

#endif

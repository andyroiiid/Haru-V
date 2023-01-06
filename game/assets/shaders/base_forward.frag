#version 450

#include <uniform_renderer.glsl>
#include <uniform_lighting.glsl>
#include <utils.glsl>
#include <pbr.glsl>

#define IBL_TEXTURE_SET 2
#include <ibl.glsl>

#define CSM_TEXTURE_SET 3
#include <csm.glsl>

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 1, binding = 0) uniform sampler2D uAlbedo;
layout (set = 1, binding = 1) uniform sampler2D uNormal;
layout (set = 1, binding = 2) uniform sampler2D uMRA;
layout (set = 1, binding = 3) uniform sampler2D uEmissive;

void main() {
    const vec4 worldPosition = vec4(vWorldPosition, 1);
    const vec4 viewSpacePosition = uView * worldPosition;
    const vec3 worldNormal = normalize(vWorldNormal);
    const vec3 emissive = texture(uEmissive, vTexCoord).rgb;

    const vec4 albedoAlpha = texture(uAlbedo, vTexCoord);
    const vec3 albedo = albedoAlpha.rgb;
    const float alpha = albedoAlpha.a;
    const vec3 mra = texture(uMRA, vTexCoord).rgb;
    const float metallic = mra.r;
    const float roughness = mra.g;
    const float ambientOcclusion = mra.b;

    const vec3 N = worldNormal;
    const vec3 V = normalize(uCameraPosition - worldPosition.xyz);
    const vec3 R = reflect(-V, N);
    const float NdotV = max(dot(N, V), 0.0);

    const vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < uNumPointLights; i++) {
        const vec3 lightDelta = uPointLights[i].Position - worldPosition.xyz;
        const float lightDistance = length(lightDelta);
        const float lightRadius = uPointLights[i].Radius;
        if (lightDistance > lightRadius) {
            continue;
        }
        const float falloff = PointLightFalloff(lightDistance, lightRadius);
        Lo += PBR(lightDelta, uPointLights[i].Color, V, N, roughness, metallic, albedo, F0) * falloff;
    }

    const float shadow = ReadShadowMap(viewSpacePosition, worldPosition);
    Lo += PBR(uLightDirection, uLightColor, V, N, roughness, metallic, albedo, F0) * (1.0 - shadow);

    const vec3 ibl = IBL(N, NdotV, R, F0, metallic, roughness, albedo, ambientOcclusion) * 0.5;

    const vec3 color = Lo * alpha + ibl + emissive;

    fColor = vec4(color, alpha);
}

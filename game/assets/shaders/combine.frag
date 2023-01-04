#version 450

#include <shaders/uniform_renderer.glsl>
#include <shaders/uniform_lighting.glsl>
#include <shaders/utils.glsl>

layout (location = 0) in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 1, binding = 0) uniform sampler2D uWorldPositionMetallic;
layout (set = 1, binding = 1) uniform sampler2D uWorldNormalRoughness;
layout (set = 1, binding = 2) uniform sampler2D uAlbedoAmbientOcclusion;
layout (set = 1, binding = 3) uniform sampler2D uEmissive;

layout (set = 2, binding = 0) uniform sampler2D uBrdfLut;
layout (set = 2, binding = 1) uniform sampler2D uSkybox;
layout (set = 2, binding = 2) uniform sampler2D uSkyboxSpecular;
layout (set = 2, binding = 3) uniform sampler2D uSkyboxIrradiance;

layout (set = 3, binding = 0) uniform sampler2DArrayShadow uShadowMap;

// PBR functions from https://learnopengl.com/PBR/Lighting

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 PBR(vec3 lightDirection, vec3 lightColor, vec3 V, vec3 N, float roughness, float metallic, vec3 albedo, vec3 F0)
{
    const vec3 L = normalize(lightDirection);
    const vec3 H = normalize(V + L);
    const float NdotL = max(dot(N, L), 0.0);
    const float NdotV = max(dot(N, V), 0.0);

    const float NDF = DistributionGGX(N, H, roughness);
    const float G = GeometrySmith(N, V, L, roughness);
    const vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    const vec3 kS = F;
    const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    const vec3 numerator = NDF * G * F;
    const float denominator = 4.0 * NdotV * NdotL + 0.0001;
    const vec3 specular = numerator / denominator;

    return (kD * albedo / PI + specular) * lightColor * NdotL;
}

// ibl textures generated using https://github.com/oframe/ibl-converter

const float ENV_LODS = 6.0;

vec3 SampleIBLReflection(vec3 R, float roughness) {
    // based on https://github.com/oframe/ibl-converter/blob/master/src/shaders/PBRShader.js
    float blend = roughness * ENV_LODS;
    float level0 = floor(blend);
    float level1 = min(ENV_LODS, level0 + 1.0);
    blend -= level0;

    vec2 uvSpec = SampleSphericalMap(R);
    uvSpec.y /= 2.0;

    vec2 uv0 = uvSpec;
    vec2 uv1 = uvSpec;

    uv0 /= pow(2.0, level0);
    uv0.y += 1.0 - exp(-LN2 * level0);

    uv1 /= pow(2.0, level1);
    uv1.y += 1.0 - exp(-LN2 * level1);

    vec3 specular0 = RGBMToLinear(texture(uSkyboxSpecular, uv0)).rgb;
    vec3 specular1 = RGBMToLinear(texture(uSkyboxSpecular, uv1)).rgb;
    vec3 reflection = mix(specular0, specular1, blend);

    return reflection;
}

vec3 IBL(vec3 N, float NdotV, vec3 R, vec3 F0, float metallic, float roughness, vec3 albedo, float ambientOcclusion) {
    const vec3 F = FresnelSchlickRoughness(NdotV, F0, roughness);
    const vec3 kS = F;
    const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    const vec3 irradiance = texture(uSkyboxIrradiance, SampleSphericalMap(N)).rgb;
    const vec3 reflection = SampleIBLReflection(R, roughness);
    const vec2 brdf = texture(uBrdfLut, vec2(NdotV, roughness)).rg;
    const vec3 specular = reflection * (F * brdf.x + brdf.y);
    return (kD * irradiance * albedo + specular) * ambientOcclusion;
}

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

vec3 ToneMapping(vec3 color) {
    const vec3 A = vec3(2.51);
    const vec3 B = vec3(0.03);
    const vec3 C = vec3(2.43);
    const vec3 D = vec3(0.59);
    const vec3 E = vec3(0.14);
    return color * fma(color, A, B) / fma(color, fma(color, C, D), E);
}

// from Real Shading in Unreal Engine 4 by Brian Karis, Epic Games
float PointLightFalloff(float distance, float radius) {
    float A = distance / radius;
    A *= A;
    A *= A;

    A = clamp(1.0 - A, 0.0, 1.0);
    A *= A;

    float B = distance * distance + 1;
    return A / B;
}

void main() {
    const vec4 worldPositionMetallic = texture(uWorldPositionMetallic, vTexCoord);
    const vec4 worldNormalRoughness = texture(uWorldNormalRoughness, vTexCoord);
    const vec4 albedoAmbientOcclusion = texture(uAlbedoAmbientOcclusion, vTexCoord);
    const vec3 emissive = texture(uEmissive, vTexCoord).rgb;

    const vec4 worldPosition = vec4(worldPositionMetallic.xyz, 1.0);
    const vec4 viewSpacePosition = uView * worldPosition;
    const vec3 worldNormal = normalize(worldNormalRoughness.xyz);
    const vec3 albedo = albedoAmbientOcclusion.rgb;

    const float metallic = worldPositionMetallic.w;
    const float roughness = worldNormalRoughness.w;
    const float ambientOcclusion = albedoAmbientOcclusion.w;

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

    const vec3 color = ToneMapping(ibl + Lo + emissive);

    fColor = vec4(color, 1);
}

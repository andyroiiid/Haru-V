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

layout (set = 2, binding = 0) uniform sampler2D uSkybox;
layout (set = 2, binding = 1) uniform sampler2D uSkyboxIrradiance;

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

vec3 GetAmbient(vec3 N, vec3 V, vec3 F0, float metallic, float roughness, vec3 albedo, float ambientOcclusion) {
    const vec3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    const vec3 kD = vec3(1.0) - kS; // probably should take metallic into account here?
    const vec3 irradiance = texture(uSkyboxIrradiance, SampleSphericalMap(N)).rgb;
    return kD * irradiance * albedo * ambientOcclusion;
}

vec3 ToneMapping(vec3 color) {
    const vec3 A = vec3(2.51);
    const vec3 B = vec3(0.03);
    const vec3 C = vec3(2.43);
    const vec3 D = vec3(0.59);
    const vec3 E = vec3(0.14);
    return color * fma(color, A, B) / fma(color, fma(color, C, D), E);
}

void main() {
    const vec4 worldPositionMetallic = texture(uWorldPositionMetallic, vTexCoord);
    const vec4 worldNormalRoughness = texture(uWorldNormalRoughness, vTexCoord);
    const vec4 albedoAmbientOcclusion = texture(uAlbedoAmbientOcclusion, vTexCoord);
    const vec3 emissive = texture(uEmissive, vTexCoord).rgb;

    const vec3 worldPosition = worldPositionMetallic.xyz;
    const vec3 worldNormal = worldNormalRoughness.xyz;
    const vec3 albedo = albedoAmbientOcclusion.rgb;

    const float metallic = worldPositionMetallic.w;
    const float roughness = worldNormalRoughness.w;
    const float ambientOcclusion = albedoAmbientOcclusion.w;

    const vec3 N = worldNormal;
    const vec3 V = normalize(uCameraPosition - worldPosition);

    const vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);

    const vec3 L = normalize(uLightDirection);
    const vec3 H = normalize(V + L);
    const vec3 radiance = uLightColor;
    const float NdotL = max(dot(N, L), 0.0);

    const float NDF = DistributionGGX(N, H, roughness);
    const float G = GeometrySmith(N, V, L, roughness);
    const vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    const vec3 kS = F;
    const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    const vec3 numerator = NDF * G * F;
    const float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
    const vec3 specular = numerator / denominator;

    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    const vec3 ambient = GetAmbient(N, V, F0, metallic, roughness, albedo, ambientOcclusion);

    const vec3 color = ToneMapping(ambient + Lo + emissive);

    fColor = vec4(color, 1);
}

#ifndef IBL_GLSL
#define IBL_GLSL

#include <pbr.glsl>

layout (set = IBL_TEXTURE_SET, binding = 0) uniform sampler2D uBrdfLut;
layout (set = IBL_TEXTURE_SET, binding = 1) uniform sampler2D uSkybox;
layout (set = IBL_TEXTURE_SET, binding = 2) uniform sampler2D uSkyboxSpecular;
layout (set = IBL_TEXTURE_SET, binding = 3) uniform sampler2D uSkyboxIrradiance;

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

#endif

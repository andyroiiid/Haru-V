#ifndef UNIFORM_LIGHTING_GLSL
#define UNIFORM_LIGHTING_GLSL

struct PointLightData {
    vec3 Position;
    float Radius;
    vec3 Color;
    float PointLightDataPadding0;
};

layout (set = 0, binding = 1) uniform LightingUniformData {
    vec3 uLightDirection;
    int uNumPointLights;
    vec3 uLightColor;
    float LightingUniformDataPadding0;
    vec3 uCascadeShadowMapSplits;
    float LightingUniformDataPadding1;
    mat4 uShadowMatrices[4];
    PointLightData uPointLights[128];
};

#endif

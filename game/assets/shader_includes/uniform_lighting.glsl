#ifndef UNIFORM_LIGHTING_GLSL
#define UNIFORM_LIGHTING_GLSL

struct PointLightData {
    vec3 Position;
    float Radius;
    vec3 Color;
    float Padding;
};

layout (set = 0, binding = 1) uniform LightingUniformData {
    vec3 uLightDirection;
    int uNumPointLights;
    vec3 uLightColor;
    float uPadding1;
    vec3 uCascadeShadowMapSplits;
    float Padding2;
    mat4 uShadowMatrices[4];
    PointLightData uPointLights[128];
};

#endif

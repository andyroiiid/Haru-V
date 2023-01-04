#version 450

#include <shaders/uniform_lighting.glsl>

layout (location = 0) in vec3 gWorldNormal;

void main()
{
    const vec3 lightDirection = normalize(uLightDirection);
    const vec3 worldNormal = normalize(gWorldNormal);
    const float slopeScale = 1 - abs(dot(lightDirection, worldNormal));
    const float bias = float[](0.002, 0.004, 0.008, 0.016)[gl_Layer];
    gl_FragDepth = gl_FragCoord.z + bias * slopeScale / gl_FragCoord.w;
}

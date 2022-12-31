#version 450

#include <shaders/uniform_lighting.glsl>

layout (location = 0) in vec3 gWorldNormal;

void main()
{
    // const vec3 lightDirection = normalize(uLightDirection);
    // const vec3 worldNormal = normalize(gWorldNormal);
    // const float slopeScale = 1 - abs(dot(lightDirection, worldNormal));
    // gl_FragDepth = gl_FragCoord.z + 0.008 * slopeScale / gl_FragCoord.w;
}

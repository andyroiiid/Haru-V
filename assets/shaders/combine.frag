#version 450

#include <shaders/uniform_lighting.glsl>

layout (location = 0) in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 1, binding = 0) uniform sampler2D uWorldPosition;
layout (set = 1, binding = 1) uniform sampler2D uWorldNormal;
layout (set = 1, binding = 2) uniform sampler2D uDiffuse;

float Lambert(vec3 worldNormal, vec3 lightDirection) {
    return max(0, dot(worldNormal, lightDirection));
}

void main() {
    vec3 worldPosition = texture(uWorldPosition, vTexCoord).xyz;
    vec3 worldNormal = normalize(texture(uWorldNormal, vTexCoord).xyz);
    vec4 color = texture(uDiffuse, vTexCoord);

    float lighting = Lambert(worldNormal, normalize(uLightDirection));
    vec3 lightingColor = mix(uAmbientColor, uLightColor, lighting);

    color.rgb *= lightingColor;

    fColor = color;
}

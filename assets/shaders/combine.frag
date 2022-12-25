#version 450

#include <shaders/uniform_lighting.glsl>
#include <shaders/utils.glsl>

layout (location = 0) in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 1, binding = 0) uniform sampler2D uWorldPositionDiffuse;
layout (set = 1, binding = 1) uniform sampler2D uWorldNormalMRA;

float Lambert(vec3 worldNormal, vec3 lightDirection) {
    return max(0, dot(worldNormal, lightDirection));
}

void main() {
    vec4 worldPositionDiffuse = texture(uWorldPositionDiffuse, vTexCoord);
    vec4 worldNormalMRA = texture(uWorldNormalMRA, vTexCoord);

    vec3 worldPosition = worldPositionDiffuse.xyz;
    vec4 diffuse = unpack(worldPositionDiffuse.w);

    vec3 worldNormal = normalize(worldNormalMRA.xyz);
    vec4 mra = unpack(worldNormalMRA.w);

    vec4 color = diffuse;

    float lighting = Lambert(worldNormal, normalize(uLightDirection));
    vec3 lightingColor = mix(uAmbientColor, uLightColor, lighting);

    color.rgb *= lightingColor;

    fColor = color;
}

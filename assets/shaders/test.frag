#version 450

layout (location = 0) in vec3 vWorldNormal;
layout (location = 1) in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 0, binding = 1) uniform LightingUniformData {
    vec3 uLightDirection;
    float uPadding0;
    vec3 uLightColor;
    float uPadding1;
    vec3 uAmbientColor;
    float uPadding2;
};

layout (set = 1, binding = 0) uniform sampler2D uTexture;

float Lambert(vec3 worldNormal, vec3 lightDirection) {
    return max(0, dot(worldNormal, lightDirection));
}

void main() {
    vec3 worldNormal = normalize(vWorldNormal);
    vec3 lightDirection = normalize(uLightDirection);

    float lighting = Lambert(worldNormal, lightDirection);
    vec3 lightingColor = mix(uAmbientColor, uLightColor, lighting);

    vec3 color = texture(uTexture, vTexCoord).rgb;
    color *= lightingColor;

    fColor = vec4(color, 1);
}

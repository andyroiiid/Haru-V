#version 450

layout (location = 0) in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 0, binding = 0) uniform sampler2D uScreen;

vec3 ToneMapping(vec3 color) {
    const vec3 A = vec3(2.51);
    const vec3 B = vec3(0.03);
    const vec3 C = vec3(2.43);
    const vec3 D = vec3(0.59);
    const vec3 E = vec3(0.14);
    return color * fma(color, A, B) / fma(color, fma(color, C, D), E);
}

void main() {
    vec3 color = texture(uScreen, vTexCoord).rgb;
    color = ToneMapping(color);
    fColor = vec4(color, 1.0);
}

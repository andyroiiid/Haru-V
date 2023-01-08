#version 450

layout (location = 0) in vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 0, binding = 0) uniform sampler2D uScreen;

vec2 AdjustLength(const vec2 v, float power) {
    const float l = length(v);
    return normalize(v) * pow(l, power);
}

vec3 ChromaticAberration(sampler2D tex, vec2 uv, float lengthAdjust, float ammount) {
    const vec2 dist = AdjustLength(uv - 0.5, lengthAdjust);

    const vec2 aberration = ammount * dist;

    return vec3(
    texture(tex, uv - aberration).r,
    texture(tex, uv).g,
    texture(tex, uv + aberration).b
    );
}

float Vignette(vec2 uv, float intensity, float extent) {
    uv *= 1.0 - uv.yx;
    return pow(uv.x * uv.y * intensity, extent);
}

vec3 ToneMapping(vec3 color) {
    const vec3 A = vec3(2.51);
    const vec3 B = vec3(0.03);
    const vec3 C = vec3(2.43);
    const vec3 D = vec3(0.59);
    const vec3 E = vec3(0.14);
    return color * fma(color, A, B) / fma(color, fma(color, C, D), E);
}

float Luma(vec3 color) {
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main() {
    vec3 color = ChromaticAberration(uScreen, vTexCoord, 1.5, 0.005);

    color *= Vignette(vTexCoord, 20.0, 0.2);

    // encode gamma corrected luma into alpha channel (so that FXAA can use)
    fColor = vec4(ToneMapping(color), sqrt(Luma(color)));
}

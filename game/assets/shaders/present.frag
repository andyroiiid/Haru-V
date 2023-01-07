#version 450

#include <uniform_renderer.glsl>

#include <fxaa.glsl>

layout (location = 0) in noperspective vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 1, binding = 0) uniform sampler2D uScreen;

void main() {
    vec2 pixelCenter = vTexCoord * (1.0 - uScreenInfo.zw) + uScreenInfo.zw * 0.5;
    fColor = Fxaa(pixelCenter, uScreen, uScreenInfo.zw, 0.75, 0.166, 0.0833);
}

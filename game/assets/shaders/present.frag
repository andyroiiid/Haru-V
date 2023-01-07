#version 450

#include <fxaa.glsl>

layout (location = 0) in noperspective vec2 vTexCoord;

layout (location = 0) out vec4 fColor;

layout (set = 0, binding = 0) uniform sampler2D uScreen;

void main() {
    fColor = Fxaa(vTexCoord, uScreen, vec2(1.0 / 1920.0, 1.0 / 1017.0), 0.75, 0.166, 0.0833);
}

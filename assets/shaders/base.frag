#version 450

layout (location = 0) in vec3 vWorldPosition;
layout (location = 1) in vec3 vWorldNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec4 fWorldPosition;
layout (location = 1) out vec4 fWorldNormal;
layout (location = 2) out vec4 fDiffuse;
layout (location = 3) out vec4 fEmissive;

layout (set = 1, binding = 0) uniform sampler2D uTexture;

void main() {
    fWorldPosition = vec4(vWorldPosition, 1);
    fWorldNormal = vec4(normalize(vWorldNormal), 0);
    fDiffuse = texture(uTexture, vTexCoord);
    fEmissive = vec4(1, 0, 0, 1);
}

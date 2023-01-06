#version 450

#include <uniform_renderer.glsl>

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 vWorldPosition;
layout (location = 1) out vec3 vWorldNormal;
layout (location = 2) out vec2 vTexCoord;

layout (push_constant) uniform PushConstantData {
    mat4 uModel;
};

void main() {
    vec4 worldPosition = uModel * vec4(aPosition, 1.0);
    gl_Position = uProjection * uView * worldPosition;
    vWorldPosition = worldPosition.xyz;
    vWorldNormal = (uModel * vec4(aNormal, 0.0)).xyz;
    vTexCoord = aTexCoord;
}

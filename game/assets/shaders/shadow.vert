#version 450

#include <uniform_renderer.glsl>

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec4 vNormal;

layout (push_constant) uniform PushConstantData {
    mat4 uModel;
};

void main() {
    gl_Position = uModel * vec4(aPosition, 1);
    vNormal = uModel * vec4(aNormal, 0);
}

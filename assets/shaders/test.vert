#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 vColor;

void main() {
    gl_Position = vec4(aPosition, 1);
    vColor = vec3(aTexCoord, 0);
}

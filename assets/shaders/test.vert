#version 450

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 vColor;

layout (set = 0, binding = 0) uniform RendererUniformData {
    mat4 uProjection;
    mat4 uView;
};

layout (push_constant) uniform PushConstantData {
    mat4 uModel;
};

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1);
    vColor = vec3(aTexCoord, 0);
}

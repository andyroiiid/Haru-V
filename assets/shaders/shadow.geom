#version 450

#include <shaders/uniform_lighting.glsl>

layout (triangles, invocations = 4) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in vec4 vNormal[];

layout (location = 0) out vec3 gWorldNormal;

void main() {
    for (int i = 0; i < 3; i++) {
        gl_Layer = gl_InvocationID;
        gl_Position = uShadowMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gWorldNormal = vNormal[i].xyz;
        EmitVertex();
    }
    EndPrimitive();
}

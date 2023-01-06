#ifndef UNIFORM_RENDERER_GLSL
#define UNIFORM_RENDERER_GLSL

layout (set = 0, binding = 0) uniform RendererUniformData {
    mat4 uProjection;
    mat4 uView;
    vec3 uCameraPosition;
};

#endif

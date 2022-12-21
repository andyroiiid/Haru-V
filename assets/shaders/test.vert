#version 450

void main() {
    const vec3 POSITIONS[3] = vec3[3](
    vec3(-1, -1, 0),
    vec3(1, -1, 0),
    vec3(0, 1, 0)
    );
    gl_Position = vec4(POSITIONS[gl_VertexIndex], 1);
}

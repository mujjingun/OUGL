R"GLSL(
#version 430 core

out vec2 vTexCoords;

void main() {
    vTexCoords = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(vTexCoords * 2.0f + -1.0f, -1.0f, 1.0f);
}
)GLSL"

#version 430 core

layout(location = 0) uniform mat4 viewMat;
layout(location = 1) uniform mat4 projMat;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 offset;

out vec2 uv;

void main() {
    uv = pos + .5;
    gl_Position = projMat * viewMat * vec4(pos + offset, -10, 1);
}

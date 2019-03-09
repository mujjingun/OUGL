#version 430 core

in vec2 uv;
in vec2 cube;

out vec3 color;

void main() {
    color = vec3(uv + .5, 1);

    if (cube.x < -1 || cube.y < -1 || cube.x > 1 || cube.y > 1)
        discard;
}

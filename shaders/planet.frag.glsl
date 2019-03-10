#version 430 core

in vec2 uv;
in vec2 cube;
in vec3 position;
in vec3 normal;
in float logz;
flat in int drawCenter;

out vec3 color;

void main() {
    if (cube.x < -1 || cube.y < -1 || cube.x > 1 || cube.y > 1) {
        discard;
    }

    if (drawCenter == 0) {
        if (uv.x >= -.5 && uv.y >= -.5 && uv.x <= .5 && uv.y <= .5) {
            discard;
        }
    }

    gl_FragDepth = logz;

    color = vec3(uv + .5, 1);

    vec3 lightDir = vec3(0, 0, 1);
    color = color * max(dot(normal, lightDir), 0.5);
    //color = (normal + 1.0) / 2.0;
}

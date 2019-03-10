#version 430 core

in vec2 vUv;
in vec2 vCube;
in vec3 vPosition;
in vec3 vNormal;
in float vLogz;
flat in vec4 vDiscardReg;

out vec3 color;

void main() {
    if (vCube.x < -1 || vCube.y < -1 || vCube.x > 1 || vCube.y > 1) {
        discard;
    }

    if (vUv.x >= vDiscardReg.x && vUv.y >= vDiscardReg.y && vUv.x <= vDiscardReg.z && vUv.y <= vDiscardReg.w) {
        discard;
    }

    gl_FragDepth = vLogz;

    color = vec3(vUv + .5, 1);

    vec3 lightDir = vec3(0, 0, 1);
    color = (vNormal + 1.0) / 2.0;
    color = color * max(dot(vNormal, lightDir), 0.5);
}

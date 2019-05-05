R"GLSL(
#version 430 core

in vec2 vUv;
in vec2 vCube;
in float vLogz;
flat in vec4 vDiscardReg;

out vec4 color;

void main() {
    // Don't render outside the face
    if (vCube.x < -1 || vCube.y < -1 || vCube.x > 1 || vCube.y > 1) {
        discard;
    }
    // Discard center region
    if (vUv.x >= vDiscardReg.x && vUv.y >= vDiscardReg.y && vUv.x <= vDiscardReg.z && vUv.y <= vDiscardReg.w) {
        discard;
    }

    gl_FragDepth = vLogz;

    color = vec4(1, 0, 0, 0.5);
}

)GLSL"

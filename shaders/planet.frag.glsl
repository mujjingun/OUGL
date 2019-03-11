#version 430 core

in vec2 vUv;
in vec2 vCube;
in vec3 vFx, vFy;
in float vLogz;
flat in float vScale;
flat in vec4 vDiscardReg;
flat in int vInstanceID;

layout(binding = 0) uniform sampler2DArray tex;

out vec3 color;

vec2 getGradient(vec2 uv, float span) {
    const float h01 = textureOffset(tex, vec3(uv, vInstanceID), ivec2(-1, 0)).x;
    const float h21 = textureOffset(tex, vec3(uv, vInstanceID), ivec2(1, 0)).x;
    const float h10 = textureOffset(tex, vec3(uv, vInstanceID), ivec2(0, -1)).x;
    const float h12 = textureOffset(tex, vec3(uv, vInstanceID), ivec2(0, 1)).x;

    const ivec3 size = textureSize(tex, 0);
    return vec2(h21 - h01, h12 - h10) * size.xy / span;
}

vec3 getNormal(vec2 uv, float span) {
    vec2 grad = getGradient(uv, span);
    vec3 normal = normalize(cross(vFx, vFy));
    vec3 fx = vFx + normal * grad.x;
    vec3 fy = vFy + normal * grad.y;
    return normalize(cross(fx, fy));
}

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

    vec2 uv = (vUv + 1.0) * .5;
    float height = texture(tex, vec3(uv, 0)).r;

    color = vec3(uv, 1.0);

    vec3 lightDir = vec3(0, 0, 1);
    vec3 normal = getNormal(uv, vScale);

    color = (normal + 1.0) / 2.0;
    //color = texture(tex, vec3(uv, 0)).rrr;
    color = color * max(dot(normal, lightDir) * 1.0, 0.1);
}

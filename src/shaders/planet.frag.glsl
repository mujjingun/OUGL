R"GLSL(
#version 430 core

in vec2 vUv;
in vec2 vCube;
in vec3 vPosition;
in vec3 vFx, vFy;
in float vLogz;
flat in float vScale;
flat in vec4 vDiscardReg;
flat in int vTexIdx;

layout(binding = 0) uniform sampler2DArray tex;
layout(rg32f, binding = 1) uniform image1D bases;

layout(location = 9) uniform float terrainFactor;

out vec3 color;

const ivec2 offsets[4] = ivec2[4](ivec2(-1, 0), ivec2(1, 0), ivec2(0, -1), ivec2(0, 1));

vec2 getGradient(vec2 uv, float t, float span) {
    vec4 h = textureGatherOffsets(tex, vec3(uv, vTexIdx), offsets, 0);
    h *= terrainFactor;

    const ivec3 size = textureSize(tex, 0);
    return vec2(h.y - h.x, h.w - h.z) * size.xy / span;
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

    vec2 t = 1 / vec2(textureSize(tex, 0));
    vec2 uv = (vUv + 1.0) * .5;
    uv = mix(t * 2.5, 1 - t * 2.5, uv);
    float height = texture(tex, vec3(uv, vTexIdx)).r;
    vec4 baseData = imageLoad(bases, vTexIdx);
    float base = baseData.r + baseData.g;
    height = max(0, base + height) * terrainFactor;

    vec2 grad = getGradient(uv, t.x, vScale);
    vec3 snormal = normalize(cross(vFx, vFy));
    vec3 fx = vFx + snormal * grad.x;
    vec3 fy = vFy + snormal * grad.y;
    vec3 normal = normalize(cross(fx, fy));
    float slope = min(dot(snormal, normal), 1.0);

    vec3 groundColor = mix(vec3(0.3, 0.3, 0.3), vec3(0.1, 0.6, 0.0), 1 - step(slope, 0.9));
    color = mix(groundColor, vec3(0.0, 0.0, 0.5), step(height, 0.0));

    const vec3 lightDir = normalize(vec3(0, 0, 1));

    float light = max(dot(normal, lightDir) * 1.0, 0.001);
    vec3 lightReflect = normalize(reflect(lightDir, normal));
    vec3 vertexToEye = normalize(vPosition);
    float specularFactor = dot(vertexToEye, lightReflect);
    specularFactor = pow(max(0, specularFactor), 40);
    specularFactor = mix(specularFactor * 0.1, specularFactor * 7, step(height, 0.0));
    light += specularFactor;

    color *= light;
}
)GLSL"

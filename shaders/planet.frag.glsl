#version 430 core

in vec2 vUv;
in vec2 vCube;
in vec3 vPosition;
in vec3 vFx, vFy;
in float vLogz;
flat in float vScale;
flat in vec4 vDiscardReg;
flat in int vTexIdx;
flat in vec2 vModOrigin;

layout(binding = 0) uniform sampler2DArray tex;

layout(location = 9) uniform float terrainFactor;

out vec3 color;

vec2 getGradient(vec2 uv, float span) {
    vec4 h = vec4(
        textureOffset(tex, vec3(uv, vTexIdx), ivec2(-1, 0)).x,
        textureOffset(tex, vec3(uv, vTexIdx), ivec2(1, 0)).x,
        textureOffset(tex, vec3(uv, vTexIdx), ivec2(0, -1)).x,
        textureOffset(tex, vec3(uv, vTexIdx), ivec2(0, 1)).x
    );
    h = max(vec4(0), h - 1.0) * terrainFactor;

    const ivec3 size = textureSize(tex, 0);
    return vec2(h.y - h.x, h.w - h.z) * size.xy / span;
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

    float t = 1 / float(textureSize(tex, 0).x);
    vec2 uv = (vUv + t) * (1.0 - t * 2);
    uv = (uv + 1.0) * .5;
    uv = fract(uv + vModOrigin);
    vec4 mapValue = texture(tex, vec3(uv, vTexIdx));
    float height = mapValue.r;
    height = max(0, height - 1.0) * terrainFactor;

    color = mix(vec3(0.1, 0.8, 0.0), vec3(0.0, 0.0, 0.7), step(height, 0.0));

    vec3 lightDir = normalize(vec3(-1, 0, 1));
    vec3 normal = getNormal(uv, vScale);

    float light = max(dot(normal, lightDir) * 1.0, 0.001);
    vec3 lightReflect = normalize(reflect(lightDir, normal));
    vec3 vertexToEye = normalize(vPosition);
    float specularFactor = dot(vertexToEye, lightReflect);
    specularFactor = pow(max(0, specularFactor), 32);
    specularFactor = mix(specularFactor * 0.1, specularFactor, step(height, 0.0));
    light += specularFactor;

    color *= light;
}

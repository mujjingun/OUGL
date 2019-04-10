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

layout(location = 9) uniform float terrainFactor;

out vec3 color;

vec2 getGradient(vec2 uv, float t, float span) {
    vec4 h = vec4(
        texture(tex, vec3(uv + ivec2(-1, 0) * t, vTexIdx)).x,
        texture(tex, vec3(uv + ivec2(1, 0)  * t, vTexIdx)).x,
        texture(tex, vec3(uv + ivec2(0, -1) * t, vTexIdx)).x,
        texture(tex, vec3(uv + ivec2(0, 1)  * t, vTexIdx)).x
    );
    h = max(vec4(0), h) * terrainFactor;

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
    vec4 mapValue = texture(tex, vec3(uv, vTexIdx));
    float height = mapValue.r;
    height = max(0, height) * terrainFactor;

    vec2 grad = getGradient(uv, t.x, vScale);
    vec3 normal = normalize(cross(vFx, vFy));
    vec3 fx = vFx + normal * grad.x;
    vec3 fy = vFy + normal * grad.y;
    normal = normalize(cross(fx, fy));
    float slope = length(grad);

    vec3 groundColor = mix(vec3(0.2, 0.14, 0.03), vec3(0.1, 0.6, 0.0), step(slope, 1.0));
    color = mix(groundColor, vec3(0.0, 0.0, 0.5), step(height, 0.0));

    const vec3 lightDir = normalize(vec3(-1, 0, 1));

    float light = max(dot(normal, lightDir) * 1.0, 0.001);
    vec3 lightReflect = normalize(reflect(lightDir, normal));
    vec3 vertexToEye = normalize(vPosition);
    float specularFactor = dot(vertexToEye, lightReflect);
    specularFactor = pow(max(0, specularFactor), 40);
    specularFactor = mix(specularFactor * 0.1, specularFactor * 7, step(height, 0.0));
    light += specularFactor;

    color *= light;
}

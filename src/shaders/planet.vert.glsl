R"GLSL(
#version 430 core

layout(std140, binding = 0) uniform Ubo {
    mat4 viewProjMat;
    vec3 xJac;
    vec3 yJac;
    vec3 xxCurv;
    vec3 xyCurv;
    vec3 yyCurv;
    vec3 eyeOffset;
    vec2 origin;
    vec2 uBase;
    int playerSide;
    float terrainFactor;
    float radius;
};

layout(binding = 1) uniform sampler2DArray tex;
layout(rg32f, binding = 2) uniform image1D bases;

// per-vertex attributes
layout(location = 0) in vec2 pos;

// per-instance attributes
layout(location = 1) in vec2 offset;
layout(location = 2) in int side;
layout(location = 3) in float scale;
layout(location = 4) in vec4 discardRegion;
layout(location = 5) in int texIdx;

out vec2 vUv;
out vec2 vCube;
out vec3 vPosition;
out vec3 vFx, vFy;
out float vLogz;
flat out float vScale;
flat out vec4 vDiscardReg;
flat out int vTexIdx;

vec3 applySide(vec3 cube, int side)
{
    switch (side) {
    case 0:
        return vec3( cube.z, cube.x, cube.y );
    case 1:
        return vec3( -cube.z, -cube.x, cube.y );
    case 2:
        return vec3( cube.y, cube.z, cube.x );
    case 3:
        return vec3( cube.y, -cube.z, -cube.x );
    case 4:
        return vec3( cube.x, cube.y, cube.z );
    case 5:
        return vec3( -cube.x, cube.y, -cube.z );
    }
}

vec3 applySide(vec2 cube, int side)
{
    return applySide(vec3(cube, 1.0), side);
}

vec3 spherizePoint(vec2 q, int side, float radius)
{
    vec3 p = applySide(q, side);
    vec3 sq = p * p;
    return vec3(
        p.x * sqrt(max(1 - sq.y / 2 - sq.z / 2 + sq.y * sq.z / 3, 0.0)),
        p.y * sqrt(max(1 - sq.z / 2 - sq.x / 2 + sq.z * sq.x / 3, 0.0)),
        p.z * sqrt(max(1 - sq.x / 2 - sq.y / 2 + sq.x * sq.y / 3, 0.0))
    ) * radius;
}

void derivative(vec2 cube, int side, out vec3 dfdx, out vec3 dfdy)
{
    vec2 sq = cube * cube;
    float t = .5 * inversesqrt(1 - sq.x / 2 - sq.y / 2 + sq.x * sq.y / 3);
    dfdx = vec3(sqrt(.5 - sq.y / 6),
                -cube.x * cube.y / 6 * inversesqrt(.5 - sq.x / 6),
                t * (2. / 3. * sq.y * cube.x - cube.x));
    dfdx = applySide(dfdx, side);

    dfdy = vec3(-cube.x * cube.y / 6 * inversesqrt(.5 - sq.y / 6),
                sqrt(.5 - sq.x / 6),
                t * (2. / 3. * sq.x * cube.y - cube.y));
    dfdy = applySide(dfdy, side);
}

void main() {
    vUv = pos;
    vDiscardReg = discardRegion;
    vTexIdx = texIdx;
    vScale = scale;

    vec2 c = pos * scale + offset;
    vec3 normal;
    if (playerSide == int(side)) {
        vCube = c + origin;

        vec3 spherized = spherizePoint(vCube, playerSide, radius);
        vec3 vBroad = spherized - spherizePoint(origin, playerSide, radius);
        vec3 vApprox = c.x * xJac + c.y * yJac + .5 * (c.x * c.x * xxCurv + 2. * c.x * c.y * xyCurv + c.y * c.y * yyCurv);
        vApprox *= radius;
        float mixFactor = smoothstep(0.0, 0.1, length(c));

        vPosition = mix(vApprox, vBroad, mixFactor);

        vec3 fxApprox = xJac + xxCurv * c.x + xyCurv * c.y;
        vec3 fyApprox = yJac + xyCurv * c.x + yyCurv * c.y;
        vec3 fxBroad, fyBroad;
        derivative(vCube, playerSide, fxBroad, fyBroad);
        vFx = mix(fxApprox, fxBroad, mixFactor);
        vFy = mix(fyApprox, fyBroad, mixFactor);

        vec3 nApprox = normalize(cross(fxApprox, fyApprox));
        normal = normalize(mix(nApprox, normalize(spherized), mixFactor));
    }
    else {
        vCube = c;

        vec3 spherized = spherizePoint(vCube, int(side), radius);
        vec3 vBroad = spherized - spherizePoint(origin, playerSide, radius);

        vPosition = vBroad;

        vec3 fxBroad, fyBroad; derivative(vCube, int(side), fxBroad, fyBroad);
        vFx = fxBroad;
        vFy = fyBroad;

        normal = normalize(spherized);
    }

    // apply heightmap
    vec2 t = 1 / vec2(textureSize(tex, 0));
    vec2 uv = (vUv + 1.0) * .5;
    uv = mix(t * 2.5, 1 - t * 2.5, uv);
    float height = texture(tex, vec3(uv, vTexIdx)).r;
    vec2 baseData = imageLoad(bases, vTexIdx).rg - uBase;
    float base = baseData.r + baseData.g;
    height = (height + base) * terrainFactor;
    vPosition += normal * radius * height;

    vPosition -= eyeOffset;

    gl_Position = viewProjMat * vec4(vPosition, 1);

    // logarithmic depth
    const float C = 1e9;
    const float far = 10000.0;
    const float FC = 1.0 / log(far * C + 1);
    vLogz = log(gl_Position.w * C + 1) * FC;
    gl_Position.z = (2 * vLogz - 1) * gl_Position.w;
}
)GLSL"
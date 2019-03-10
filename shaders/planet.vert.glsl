#version 430 core

layout(location = 0) uniform mat4 viewProjMat;
layout(location = 1) uniform vec2 origin;
layout(location = 2) uniform vec3 xJac;
layout(location = 3) uniform vec3 yJac;
layout(location = 4) uniform vec3 xxCurv;
layout(location = 5) uniform vec3 xyCurv;
layout(location = 6) uniform vec3 yyCurv;
layout(location = 7) uniform int playerSide;

// per-vertex attributes
layout(location = 0) in vec2 pos;

// per-instance attributes
layout(location = 1) in vec2 offset;
layout(location = 2) in float side;
layout(location = 3) in float scale;
layout(location = 4) in vec4 discardRegion;

out vec2 vUv;
out vec2 vCube;
out vec3 vPosition;
out vec3 vNormal;
out float vLogz;
flat out vec4 vDiscardReg;

vec3 applySide(vec2 cube, int side)
{
    switch (side) {
    case 0:
        return vec3( 1, cube.x, cube.y );
    case 1:
        return vec3( -1, -cube.x, cube.y );
    case 2:
        return vec3( cube.y, 1, cube.x );
    case 3:
        return vec3( cube.y, -1, -cube.x );
    case 4:
        return vec3( cube.x, cube.y, 1 );
    case 5:
        return vec3( -cube.x, cube.y, -1 );
    }
}

vec3 spherizePoint(vec3 p)
{
    vec3 sq = p * p;
    return vec3(
        p.x * sqrt(max(1 - sq.y / 2 - sq.z / 2 + sq.y * sq.z / 3, 0.0)),
        p.y * sqrt(max(1 - sq.z / 2 - sq.x / 2 + sq.z * sq.x / 3, 0.0)),
        p.z * sqrt(max(1 - sq.x / 2 - sq.y / 2 + sq.x * sq.y / 3, 0.0))
    );
}

void main() {
    vUv = pos;
    vDiscardReg = discardRegion;

    vec2 c = pos * scale + offset;
    if (playerSide == int(side)) {
        vec3 spherized = spherizePoint(applySide(c + origin, int(side)));
        vec3 vBroad = spherized - spherizePoint(applySide(origin, playerSide));
        vec3 vApprox = c.x * xJac + c.y * yJac + .5 * (c.x * c.x * xxCurv + 2. * c.x * c.y * xyCurv + c.y * c.y * yyCurv);
        float mixFactor = smoothstep(0.0, 0.1, length(c));

        vCube = c + origin;
        vPosition = mix(vApprox, vBroad, mixFactor);
        vNormal = mix(normalize(cross(xJac, yJac)), normalize(spherized), mixFactor);
    }
    else {
        vec3 spherized = spherizePoint(applySide(c, int(side)));
        vec3 vBroad = spherized - spherizePoint(applySide(origin, playerSide));

        vCube = c;
        vPosition = vBroad;
        vNormal = normalize(spherized);
    }

    gl_Position = viewProjMat * vec4(vPosition, 1);

    // logarithmic depth
    const float C = 1;
    const float far = 10000.0;
    const float FC = 1.0 / log(far * C + 1);
    vLogz = log(gl_Position.w * C + 1) * FC;
    gl_Position.z = vLogz * gl_Position.w;
}

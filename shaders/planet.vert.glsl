#version 430 core

layout(location = 0) uniform mat4 viewMat;
layout(location = 1) uniform mat4 projMat;
layout(location = 2) uniform vec2 origin;
layout(location = 3) uniform vec3 xJac;
layout(location = 4) uniform vec3 yJac;
layout(location = 5) uniform vec3 xxCurv;
layout(location = 6) uniform vec3 xyCurv;
layout(location = 7) uniform vec3 yyCurv;

layout(location = 0) in vec2 pos;

layout(location = 1) in vec2 offset;
layout(location = 2) in float side;
layout(location = 3) in float scale;

out vec2 uv;
out vec2 cube;

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
    uv = pos;

    vec2 c = pos + offset;
    cube = c + origin;
    vec3 vApprox = c.x * xJac + c.y * yJac + .5 * (c.x * c.x * xxCurv + 2. * c.x * c.y * xyCurv + c.y * c.y * yyCurv);
    vec3 vBroad = spherizePoint(applySide(cube, int(side)));
    vec3 v = mix(vApprox, vBroad, smoothstep(0.0, 0.5, length(c)));

    gl_Position = projMat * viewMat * vec4(v, 1);
}

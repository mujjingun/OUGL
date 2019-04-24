R"GLSL(
#version 430
#define WORKGROUP_SIZE 32
layout(local_size_x = WORKGROUP_SIZE, local_size_y = WORKGROUP_SIZE, local_size_z = 1) in;
layout(r32f, binding = 0) uniform image2DArray image;

#define DECL_FASTMOD_N(n, k) vec##k mod##n(vec##k x) { return x - floor(x * (1.0 / n)) * n; }

DECL_FASTMOD_N(289, 2)
DECL_FASTMOD_N(289, 3)
DECL_FASTMOD_N(289, 4)

// Permutation polynomial: (34x^2 + x) mod 289
vec4 permute(vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v) {
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289(i);
    vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

float ridgeNoise(vec3 v)
{
    return 2 * (.5 - abs(0.5 - snoise(v)));
    //return -abs(abs(2 * snoise(v)) - 1) + 1;
}

float ridgeWithOctaves(vec3 v, int n)
{
    float F = 1;
    float coeff = 1.0f;
    for (int i = 0; i < n; ++i) {
        float t = ridgeNoise(v * coeff) / coeff;
        t = sign(t) * pow(abs(t), 0.9f);
        F += t * F;
        coeff *= 2;
    }
    F = sign(F) * pow(abs(F), 1.3f);
    return F;
}

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

vec3 spherizePoint(vec2 q, int side)
{
    vec3 p = applySide(q, side);
    vec3 sq = p * p;
    return vec3(
        p.x * sqrt(max(1 - sq.y / 2 - sq.z / 2 + sq.y * sq.z / 3, 0.0)),
        p.y * sqrt(max(1 - sq.z / 2 - sq.x / 2 + sq.z * sq.x / 3, 0.0)),
        p.z * sqrt(max(1 - sq.x / 2 - sq.y / 2 + sq.x * sq.y / 3, 0.0))
    );
}

#define MARGIN 2

void main() {
    ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz);
    vec2 imgSize = vec2(imageSize(image).xy);
    vec2 t = 1 / imgSize;

    // map [MARGIN, N-1-MARGIN] -> [1/(2N), 1-1/(2N)]
    vec2 uv = mix(t * .5, 1 - t * .5, vec2(pixel_coords.xy - MARGIN) / (imgSize - (MARGIN * 2 + 1)));
    vec2 xy = uv * 2. - 1.;
    vec3 pos = spherizePoint(xy, pixel_coords.z);

    float height = (ridgeWithOctaves(pos * 2.0, 20) - 1.0);
    vec4 pixel = vec4(height, 0.0, 0.0, 1.0);

    // output to a specific pixel in the image
    imageStore(image, pixel_coords, pixel);
}
)GLSL"

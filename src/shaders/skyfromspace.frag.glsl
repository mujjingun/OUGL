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
    vec3 lightDir;
    vec3 eyePos;
    vec2 origin;
    vec2 uBase;
    int playerSide;
    float terrainFactor;
    float innerRadius;
};

in vec2 vCube;
in float vLogz;
in vec3 vPosition;

in vec3 vC0; // rayleigh color
in vec3 vC1; // mie color

out vec4 color;

// Calculates the Mie phase function
float getMiePhase(float fCos, float fCos2, float g, float g2)
{
    return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
}

// Calculates the Rayleigh phase function
float getRayleighPhase(float fCos2)
{
    //return 1.0;
    return 0.75 + 0.75*fCos2;
}

const float g = -0.990; // The Mie phase asymmetry factor
const float g2 = g * g;

void main() {
    gl_FragDepth = vLogz;

    vec3 direction = vPosition;
    float fCos = dot(lightDir, direction) / length(direction);
    float fCos2 = fCos * fCos;
    color.rgb = getRayleighPhase(fCos2) * vC0 + getMiePhase(fCos, fCos2, g, g2) * vC1;
    color.a = 0;
}

)GLSL"

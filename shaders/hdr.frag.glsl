#version 430 core

in vec2 vTexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

out vec4 FragColor;

void main()
{
    vec3 hdrColor = texture(screenTexture, vTexCoords).rgb;

    // exposure tone mapping
    const float exposure = 1.0;
    vec3 toneMapped = 1.0 - exp(-hdrColor * exposure);

    // gamma correction
    const float gamma = 2.2;
    vec3 color = pow(toneMapped, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}

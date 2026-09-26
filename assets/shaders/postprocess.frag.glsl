#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D hdrScene;
uniform float exposure;

void main()
{
    vec3 hdrColor = texture(hdrScene, TexCoord).rgb;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    vec3 gammaCorrected = pow(max(mapped, vec3(0.0)), vec3(1.0 / 2.2));
    FragColor = vec4(gammaCorrected, 1.0);
}

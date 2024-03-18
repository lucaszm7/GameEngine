#version 440 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 texCol = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(texCol, 1.0);
}
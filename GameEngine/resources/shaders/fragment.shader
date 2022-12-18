#version 440 core

in vec3 outColor;
in vec2 outTexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, outTexCoord) * vec4(outColor, 1.0);
}

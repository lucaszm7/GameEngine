#version 440 core

in vec3 outColor;
in vec2 outTexCoord;

out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float smilePercentage;

void main()
{
	FragColor = mix(texture(texture0, outTexCoord), texture(texture1, outTexCoord), smilePercentage);
}

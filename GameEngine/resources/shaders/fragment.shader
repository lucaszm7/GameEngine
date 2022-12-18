#version 440 core

in vec3 outColor;
in vec2 outTexCoord;

out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float smilePercentage;

void main()
{
	vec4 finalColor0 = mix(texture(texture0, outTexCoord), texture(texture1, outTexCoord), smilePercentage);
	vec4 finalColor1 = mix(texture(texture0, outTexCoord), texture(texture2, outTexCoord), smilePercentage);

	FragColor = mix(finalColor0, finalColor1, 0.5) * vec4(outColor, 1.0);
}

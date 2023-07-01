#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 outNormal;
out vec3 outFragPos;
out vec2 outTexCoord;

out vec3 outColor;

uniform vec3 uColor;

void main()
{
	outNormal = aNormal; // mat3(transpose(inverse(model))) * 
	outTexCoord = aTexCoord;
    outColor = uColor;
	outFragPos = aPos;
	gl_Position = vec4(aPos, 1.0);
}
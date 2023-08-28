#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

out vec3 outNormal;
out vec3 outFragPos;
out vec3 outColor;
out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	outFragPos = vec3(model * vec4(aPos, 1.0));
	outNormal = mat3(transpose(inverse(model))) * aNormal;
	outTexCoord = aTexCoord;
    outColor = aColor;
											   // local
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
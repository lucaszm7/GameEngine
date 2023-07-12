#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 outColor;

void main()
{									   // World
	gl_Position = projection * view * vec4(aPos, 1.0);
	outColor = aColor;
}
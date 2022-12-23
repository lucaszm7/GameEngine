#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec3 outColor;
out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{											   // local
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	outColor = vec3(1.0, 0.0, 0.0);
	outTexCoord = aTexCoord;
}
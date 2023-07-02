#version 440 core

layout(location = 0) in vec4 aPos;

out vec3 outColor;
uniform vec3 uColor;

void main()
{
    outColor = uColor;
	gl_Position = aPos;
}
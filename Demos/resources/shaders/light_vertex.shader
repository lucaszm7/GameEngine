#version 440 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor;

out vec3 outColor;

void main()
{											   // local
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	outColor = vec3(lightColor.y, lightColor.z, lightColor.x);
}
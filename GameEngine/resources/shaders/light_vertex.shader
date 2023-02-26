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
	if (gl_VertexID % 2 == 0)
		outColor = lightColor;
	else
		outColor = vec3(lightColor.y, lightColor.z, lightColor.x);
}
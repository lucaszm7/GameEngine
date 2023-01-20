#version 440 core

in vec3 outNormal;
in vec3 outFragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(outNormal);
	vec3 lightDir = normalize(lightPos - outFragPos);

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	float diffuseStrenght = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseStrenght * lightColor;

	float specularStrenght = 0.5;
	vec3 viewDir = normalize(viewPos - outFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrenght * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);

}

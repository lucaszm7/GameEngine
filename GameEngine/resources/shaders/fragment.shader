#version 440 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

uniform Material material;
uniform Light light;

in vec3 outNormal;
in vec3 outFragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(outNormal);
	vec3 lightDir = normalize(light.position - outFragPos);

	vec3 ambient = material.ambient * light.ambient;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * material.diffuse) * light.diffuse;

	vec3 viewDir = normalize(viewPos - outFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (material.specular * spec) * light.specular;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);

}

#version 440 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Light
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

in vec3 outNormal;
in vec3 outFragPos;
in vec2 outTexCoord;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(outNormal);
	vec3 lightDir = normalize(light.position - outFragPos);

	vec3 ambient = light.ambient * texture(material.diffuse, outTexCoord).rgb;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, outTexCoord).rgb;

	vec3 viewDir = normalize(viewPos - outFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

	vec3 emission = texture(material.emission, outTexCoord).rgb;

	vec3 result = ambient + diffuse + specular + emission;
	FragColor = vec4(result, 1.0);
}

#version 440 core

#define NR_MAX_DIFFUSE  3
#define NR_MAX_SPECULAR 2
#define NR_MAX_EMISSION 1

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct DirectionalLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Spotlight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 outNormal;
in vec3 outFragPos;
in vec2 outTexCoord;
in vec3 outColor;

out vec4 FragColor;

uniform Material material;
uniform vec3 viewPos;

uniform DirectionalLight fragmentDirectionalLight;
#define NR_POINT_LIGHTS 4  
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Spotlight fragmentSpotlight;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);
vec3 CalculateSpotlight (Spotlight  light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(outNormal);
	vec3 viewDir = normalize(viewPos - outFragPos);
	vec3 result = vec3(0);

	// Directional Light
    result += CalculateDirectionalLight(fragmentDirectionalLight, norm, viewDir);

	// Point Light
	for (int i = 0; i < 1; ++i)
		result += CalculatePointLight(pointLights[i], norm, outFragPos, viewDir);

	// Spotlight
    result += CalculateSpotlight(fragmentSpotlight, norm, outFragPos, viewDir);

	// Emission map
	// result += texture(material.emission, outTexCoord).rgb;

    FragColor = vec4(result, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	// diffuse shading
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

	return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	// Ambient light
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));

	// Diffuse light
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));

	// Specular light
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

	// Point light attenuation
	float distance = length(light.position - outFragPos);
	float attenuation = 1.0 /
		(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// combine results
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

vec3 CalculateSpotlight(Spotlight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	// Ambient light
	vec3 ambient = light.ambient * texture(material.diffuse, outTexCoord).rgb;

	// Diffuse light
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, outTexCoord).rgb;

	// Specular light
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

	// Point light attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 /
		(light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	ambient  *= intensity * attenuation;
	diffuse  *= intensity * attenuation;
	specular *= intensity * attenuation;

	return (ambient + diffuse + specular);
}

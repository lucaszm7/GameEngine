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
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float quadratic;
	float linear;
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
	float quadratic;
	float linear;
};

in vec3 outNormal;
in vec3 outFragPos;
in vec2 outTexCoord;

in vec3 outColor;
in vec3 outViewPos;

out vec4 FragColor;

uniform Material material;

uniform DirectionalLight fragmentDirectionalLight;
uniform Spotlight fragmentSpotlight;

subroutine vec3 Shading(vec3 normal, vec3 viewDir);
subroutine uniform Shading shadingSelected;

vec3 None    (vec3 normal, vec3 viewDir);
vec3 Gouraud (vec3 normal, vec3 viewDir);
vec3 Phong   (vec3 normal, vec3 viewDir);

subroutine vec3 Coloring(bool isDiffuse);
subroutine uniform Coloring coloringSelected;

vec3 SolidColor   (bool isDiffuse);
vec3 TextureColor (bool isDiffuse);

vec3 PhongDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 PhongPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);
vec3 PhongSpotlight(Spotlight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(outNormal);
    vec3 viewDir = normalize(outViewPos - outFragPos);
    vec3 result = shadingSelected(norm, viewDir);

	FragColor = vec4(result, 1.0);
}

subroutine (Coloring)
vec3 SolidColor(bool isDiffuse)
{
    return outColor;
}

subroutine (Coloring)
vec3 TextureColor(bool isDiffuse)
{
    return isDiffuse ? vec3(texture(material.diffuse, outTexCoord)) : vec3(texture(material.specular, outTexCoord));
}

subroutine (Shading) 
vec3 None(vec3 normal, vec3 viewDir)
{
    return outColor;
}

subroutine (Shading) 
vec3 Gouraud(vec3 normal, vec3 viewDir)
{
    return outColor;
}

subroutine (Shading) 
vec3 Phong(vec3 normal, vec3 viewDir)
{
    vec3 result = vec3(0.0);
	
	// Directional Light
    result += PhongDirectionalLight(fragmentDirectionalLight, normal, viewDir);

	// Point Light
	/*for (int i = 0; i < NR_POINT_LIGHTS; ++i)
		result += CalculatePointLight(pointLights[i], norm, outFragPos, viewDir);*/

	// Spotlight
    result += PhongSpotlight(fragmentSpotlight, normal, outFragPos, viewDir);

	// Emission map
	// result += texture(material.emission, outTexCoord).rgb;
	
    return result;
}

vec3 PhongDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	// diffuse shading
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);

	// combine results
    vec3 ambient = light.ambient * coloringSelected(true);
    vec3 diffuse = light.diffuse * diff * coloringSelected(true);
    vec3 specular = light.specular * spec * coloringSelected(false);

	return (ambient + diffuse + specular);
}

vec3 PhongPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	// Ambient light
    vec3 ambient = light.ambient * coloringSelected(true);

	// Diffuse light
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * coloringSelected(true);

	// Specular light
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = light.specular * spec * coloringSelected(false);

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

vec3 PhongSpotlight(Spotlight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	// Ambient light
    vec3 ambient = light.ambient * coloringSelected(true);

	// Diffuse light
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * coloringSelected(true);

	// Specular light
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = light.specular * spec * coloringSelected(false);

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

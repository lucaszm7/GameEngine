#version 440 core

struct DirectionalLight
{
    vec3 direction;

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
    float quadratic;
	float linear;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

out vec3 outNormal;
out vec3 outFragPos;
out vec2 outTexCoord;
out vec3 outViewPos;

out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

uniform DirectionalLight vertexDirectionalLight;
uniform Spotlight vertexSpotlight;

subroutine vec3 Shading(vec3 pos, vec3 normal, vec3 viewDir);
subroutine uniform Shading shadingSelected;

vec3 None   (vec3 pos, vec3 normal, vec3 viewDir);
vec3 Gouraud(vec3 pos, vec3 normal, vec3 viewDir);
vec3 Phong  (vec3 pos, vec3 normal, vec3 viewDir);

vec3 GouraudDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 GouraudSpotlight(Spotlight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{
	outFragPos = vec3(model * vec4(aPos, 1.0));
	outNormal = mat3(transpose(inverse(model))) * aNormal;
	outTexCoord = aTexCoord;
    outViewPos = viewPos;
    
    vec3 norm = normalize(outNormal);
    vec3 viewDir = normalize(viewPos - outFragPos);
    
    outColor = shadingSelected(outFragPos, norm, viewDir);
											   // local
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

subroutine (Shading) 
vec3 None(vec3 pos, vec3 normal, vec3 viewDir)
{
    return aColor;
}

subroutine (Shading) 
vec3 Phong(vec3 pos, vec3 normal, vec3 viewDir)
{
    return aColor;
}

subroutine (Shading) 
vec3 Gouraud(vec3 pos, vec3 normal, vec3 viewDir)
{
    vec3 result = vec3(0.0);
	// Directional Light
    result += GouraudDirectionalLight(vertexDirectionalLight, normal, viewDir);

	// Point Light
	/*for (int i = 0; i < NR_POINT_LIGHTS; ++i)
		result += CalculatePointLight(pointLights[i], norm, outFragPos, viewDir);*/

	// Spotlight
    result += GouraudSpotlight(vertexSpotlight, normal, pos, viewDir);

	// Emission map
	// result += texture(material.emission, outTexCoord).rgb;
	
    return result;
}

vec3 GouraudDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    // diffuse shading
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);

	// combine results
    vec3 ambient = light.ambient * aColor;
    vec3 diffuse = light.diffuse * diff * aColor;
    vec3 specular = light.specular * spec * aColor;

    return (ambient + diffuse + specular);
}

vec3 GouraudSpotlight(Spotlight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
	// Ambient light
    vec3 ambient = light.ambient * aColor;

	// Diffuse light
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * aColor;

	// Specular light
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = light.specular * spec * aColor;

	// Point light attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 /
		(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    ambient *= intensity * attenuation;
    diffuse *= intensity * attenuation;
    specular *= intensity * attenuation;

    return (ambient + diffuse + specular);
}

#pragma once

#include <GLM/glm.hpp>

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess = 0.5f;
};
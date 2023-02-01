#pragma once

#include <GLM/glm.hpp>

struct Light
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 position;

	Light(glm::vec3 lightColor, glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f))
	{
		this->diffuse  = lightColor * glm::vec3(0.5f);
		this->ambient  = this->diffuse * glm::vec3(0.2f);
		this->specular = lightColor;
		this->position = pos;
	}

	void SetLightColor(glm::vec3 lightColor)
	{
		this->diffuse = lightColor * glm::vec3(0.5f);
		this->ambient = this->diffuse * glm::vec3(0.2f);
		this->specular = lightColor;
	}

	inline glm::vec3 GetLightColor() const
	{
		return this->specular;
	}

};


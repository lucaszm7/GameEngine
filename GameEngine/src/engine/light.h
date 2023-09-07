#pragma once

#include <GLM/glm.hpp>
#include <IMGUI/imgui.h>

struct Light
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 lightColor;

	Light() = default;

	Light(glm::vec3 lightColor)
		:lightColor(lightColor)
	{
		SetLightColor();
	}

	void SetLightColor(glm::vec3 lightCol)
	{
		this->lightColor = lightCol;
		SetLightColor();
	}

	void SetLightColor()
	{
		this->ambient  = this->lightColor * glm::vec3(0.2f);
		this->diffuse  = this->lightColor * glm::vec3(0.5f);
		this->specular = this->lightColor * glm::vec3(1.0f);
	}

	glm::vec3 GetLightColor() const
	{
		return this->specular;
	}
};

struct DirectionalLight : public Light
{
	glm::vec3 direction = { 0.0f, 0.0f, -1.0f };

	DirectionalLight() = default;

	DirectionalLight(glm::vec3 lightColor, glm::vec3 dir)
		: Light(lightColor), direction(dir) {}
};

struct PointLight : public Light
{
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	float intensity;

	PointLight(glm::vec3 lightColor, glm::vec3 pos, float cons = 1.0f, float lin = 0.09f, float quad = 0.032f, float inte = 1.0f)
		: Light(lightColor), position(pos), constant(cons), linear(lin), quadratic(quad), intensity(inte) {}

	void OnImGui(const std::string& name)
	{
		ImGui::DragFloat(std::string("Int " + name).c_str(), &intensity);
		ImGui::DragFloat3(std::string("Pos " + name).c_str(), &position[0]);
		ImGui::ColorEdit3(std::string("Cor " + name).c_str(), &lightColor[0]);

		SetLightColor();
	}
};

struct SpotLight : public Light
{
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	float intensity;

	void OnImGui(const std::string& name)
	{
		ImGui::DragFloat(std::string("Int " + name).c_str(), &intensity);
		ImGui::DragFloat3(std::string("Pos " + name).c_str(), &position[0]);
		ImGui::ColorEdit3(std::string("Cor " + name).c_str(), &lightColor[0]);

		SetLightColor();
	}

	SpotLight(glm::vec3 lightColor, glm::vec3 pos, glm::vec3 dir, float cut = glm::cos(glm::radians(12.5f)), float outerCut = glm::cos(glm::radians(17.5f)),
			  float cons = 1.0f, float lin = 0.09f, float quad = 0.032f, float inte = 1.0f)
		: Light(lightColor), position(pos), direction(dir), cutOff(cut), outerCutOff(outerCut), constant(cons), linear(lin), quadratic(quad), intensity(inte) {}
};
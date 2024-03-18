#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ComputeShader
{
public:
	ComputeShader(const std::string& filepath);
	~ComputeShader();

	void Bind() const;
	void Unbind() const;

	void Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const;

	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, const glm::vec3& v);
	void SetUniform2f(const std::string& name, const glm::vec2& v);
	void SetUniform1d(const std::string& name, double v0);
	void SetUniform1i(const std::string& name, int v0);
	void SetUniform1f(const std::string& name, float v0);
	void SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat4);

private:
	unsigned int m_ID;
	int GetUniformLocation(const std::string& name);
	std::string ParseShader(const std::string& filepath) const;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};


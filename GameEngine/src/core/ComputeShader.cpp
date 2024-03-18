#include "ComputeShader.h"
#include <glm/gtc/type_ptr.hpp>

ComputeShader::ComputeShader(const std::string& filepath)
{
	std::string source = ParseShader(filepath);
	const char* src = source.c_str();

	unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);
		std::cout << "Failed to compile Compute Shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(shader);
	}

	m_ID = glCreateProgram();
	glAttachShader(m_ID, shader);
	glLinkProgram(m_ID);
	glValidateProgram(m_ID);

	glDeleteShader(shader);
}

ComputeShader::~ComputeShader()
{
}

std::string ComputeShader::ParseShader(const std::string& filepath) const
{
	std::ifstream stream(filepath);

	if (!stream)
	{
		std::cout << "Wrong shader Path!" << std::endl;
		return "";
	}

	std::stringstream ss;
	ss << stream.rdbuf();
	stream.close();

	return ss.str();
}

void ComputeShader::Bind() const
{
	glUseProgram(m_ID);
}

void ComputeShader::Unbind() const
{
	glUseProgram(0);
}

void ComputeShader::Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const
{
	Bind();
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
	Unbind();
}

void ComputeShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void ComputeShader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void ComputeShader::SetUniform3f(const std::string& name, const glm::vec3& v)
{
	glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
}

void ComputeShader::SetUniform2f(const std::string& name, const glm::vec2& v)
{
	glUniform2f(GetUniformLocation(name), v.x, v.y);
}

void ComputeShader::SetUniform1d(const std::string& name, double v0)
{
	glUniform1d(GetUniformLocation(name), v0);
}

void ComputeShader::SetUniform1i(const std::string& name, int v0)
{
	glUniform1i(GetUniformLocation(name), v0);
}

void ComputeShader::SetUniform1f(const std::string& name, float v0)
{
	glUniform1f(GetUniformLocation(name), v0);
}

void ComputeShader::SetUniformMatrix4fv(const std::string& name, const glm::mat4& mat4)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
}

int ComputeShader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.contains(name))
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location == -1)
		std::cout << "\033[1;33m" << "[WARNING]\nUniform " << "\033[1;34m" << name << "\033[1;33m" << " was not found" << "\033[0m" << "\n";

	m_UniformLocationCache[name] = location;
	return location;
}
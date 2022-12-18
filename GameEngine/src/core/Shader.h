#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <GL/glew.h>


class Shader
{
private:
	std::string m_VertexShaderPath;
	std::string m_FragmentShaderPath;

	// Program ID
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Shader();
	// We call bind here just for consistency, but in behin its actully
	// gl_useProgram();
	void Bind() const;
	void Unbind() const;
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1i(const std::string& name, int v0);
	void SetUniform1f(const std::string& name, float v0);
	// void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
	std::string ParseShader(const std::string& filepath) const;
	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) const;
	int GetUniformLocation(const std::string& name);
};